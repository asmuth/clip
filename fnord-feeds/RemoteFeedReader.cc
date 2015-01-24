/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-feeds/RemoteFeedReader.h"
#include "fnord/base/logging.h"
#include "fnord/json/JSONRPCCodec.h"
#include "fnord-feeds/FeedService.h"

namespace fnord {
namespace feeds {

// TODO: retry timeout after error?

RemoteFeedReader::RemoteFeedReader(
    RPCClient* rpc_client) :
    rpc_client_(rpc_client),
    time_backfill_fn_(nullptr) {}

void RemoteFeedReader::addSourceFeed(
    URI rpc_url,
    String feed_name,
    uint64_t initial_offset,
    size_t batch_size /* = kDefaultBatchSize */,
    size_t max_buffer_size /* = kDefaultMaxBufferSize */) {
  for (const auto& source : sources_) {
    if (source->feed_name == feed_name) {
      RAISEF(kIndexError, "feed '$0' already exists", feed_name);
    }
  }

  auto source = new SourceFeed();
  source->rpc_url = rpc_url;
  source->feed_name = feed_name;
  source->batch_size = batch_size;
  source->max_buffer_size = max_buffer_size;
  source->is_fetching = false;
  source->next_offset = initial_offset;
  source->consumed_offset = initial_offset;
  source->stream_time = 0;
  sources_.emplace_back(source);
}

Option<FeedEntry> RemoteFeedReader::fetchNextEntry() {
  ScopedLock<std::mutex> lk(mutex_);
  int idx = -1;
  uint64_t min_stream_time = std::numeric_limits<uint64_t>::max();
  Duration max_spread(3600 * kMicrosPerSecond);

  for (int i = 0; i < sources_.size(); ++i) {
    const auto& source = sources_[i];

    maybeFillBuffer(source.get());

    if (source->stream_time.unixMicros() < min_stream_time) {
      min_stream_time = source->stream_time.unixMicros();

      if (source->read_buffer.size() > 0) {
        idx = i;
      } else {
        idx = -1;
      }
    }
  }

  if (idx == -1) {
    min_stream_time += max_spread.microseconds();

    for (int i = 0; i < sources_.size(); ++i) {
      const auto& source = sources_[i];

      if (source->stream_time.unixMicros() < min_stream_time &&
          source->read_buffer.size() > 0) {
        min_stream_time = source->stream_time.unixMicros();
        idx = i;
      }
    }
  }

  if (idx < 0) {
    return None<FeedEntry>();
  } else {
    const auto& source = sources_[idx];
    auto entry = source->read_buffer.front();
    source->read_buffer.pop_front();

    if (entry.time > source->stream_time) {
      source->stream_time = entry.time;
    }

    source->consumed_offset = entry.next_offset;
    return Some(entry);
  }
}

void RemoteFeedReader::maybeFillBuffer(SourceFeed* source) {
  if (source->is_fetching ||
      source->read_buffer.size() >= source->max_buffer_size) {
    return;
  }

  source->is_fetching = true;

#ifndef NDEBUG
  fnord::logTrace(
      "fnord.feeds.remotefeedreader",
      "Fetching from feed\n    name=$0\n    url=$1\n    offset=$2",
      source->feed_name,
      source->rpc_url.toString(),
      source->next_offset);
#endif

  auto rpc = fnord::mkRPC<json::JSONRPCCodec>(
      &FeedService::fetch,
      source->feed_name,
      source->next_offset,
      (int) source->batch_size);

  rpc_client_->call(source->rpc_url, rpc.get());

  rpc->onSuccess([this, source] (const decltype(rpc)::ValueType& r) mutable {
    ScopedLock<std::mutex> lk(mutex_);

    for (const auto& e : r.result()) {
      auto entry = e;

      if (time_backfill_fn_) {
        entry.time = time_backfill_fn_(entry);
      }

      source->read_buffer.emplace_back(std::move(entry));
    }

    source->is_fetching = false;

    if (source->read_buffer.size() > 0) {
      source->next_offset = source->read_buffer.back().next_offset;
      lk.unlock();
      data_available_wakeup_.wakeup();
    }
  });

  rpc->onError([this, source] (const Status& status) {
    ScopedLock<std::mutex> lk(mutex_);
    source->is_fetching = false;
    lk.unlock();

    logError(
        "fnord.feeds.remotefeedreader",
        "Error while fetching from feed:\n" \
        "    feed=$1\n    url=$0\n    error=$2",
        source->rpc_url.toString(),
        source->feed_name,
        status);

    data_available_wakeup_.wakeup();
  });
}

void RemoteFeedReader::waitForNextEntry() {
  ScopedLock<std::mutex> lk(mutex_);
  bool is_data_available = false;

  for (const auto& source : sources_) {
    maybeFillBuffer(source.get());

    if (source->read_buffer.size() > 0) {
      is_data_available = true;
    }
  }

  /* fastpath if there is data available on any feed */
  if (is_data_available) {
    return;
  }

  auto wakeup_gen = data_available_wakeup_.generation();
  lk.unlock();

  /* wait until there is any data available */
  data_available_wakeup_.waitForWakeup(wakeup_gen);
}

Vector<Pair<String, uint64_t>> RemoteFeedReader::streamOffsets() const {
  ScopedLock<std::mutex> lk(mutex_);

  Vector<Pair<String, uint64_t>> offsets;
  for (const auto& source : sources_) {
    offsets.emplace_back(source->feed_name, source->consumed_offset);
  }

  return offsets;
}

Pair<DateTime, DateTime> RemoteFeedReader::watermarks() const {
  ScopedLock<std::mutex> lk(mutex_);

  if (sources_.size() == 0) {
    return std::make_pair(DateTime(0), DateTime(0));
  }

  uint64_t low = std::numeric_limits<uint64_t>::max();
  uint64_t high = 0;

  for (const auto& source : sources_) {
    if (source->stream_time.unixMicros() < low) {
      low = source->stream_time.unixMicros();
    }

    if (source->stream_time.unixMicros() > high) {
      high = source->stream_time.unixMicros();
    }
  }

  return std::make_pair(DateTime(low), DateTime(high));
}

DateTime RemoteFeedReader::lowWatermark() const {
  return watermarks().first;
}

DateTime RemoteFeedReader::highWatermark() const {
  return watermarks().second;
}

void RemoteFeedReader::setTimeBackfill(
    Function<DateTime (const FeedEntry& entry)> fn) {
  time_backfill_fn_ = fn;
}

void RemoteFeedReader::exportStats(
    const String& path_prefix /* = "/fnord/feeds/reader/" */,
    stats::StatsRepository* stats_repo /* = nullptr */) {
}

}
}

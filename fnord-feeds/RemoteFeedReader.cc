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

RemoteFeedReader::RemoteFeedReader(
    RPCClient* rpc_client) :
    rpc_client_(rpc_client) {}

void RemoteFeedReader::addSourceFeed(
    URI rpc_url,
    String feed_name,
    size_t max_buffer_size /* = kDefaultMaxBufferSize */) {
  auto source = new SourceFeed();
  source->rpc_url = rpc_url;
  source->feed_name = feed_name;
  source->max_buffer_size = max_buffer_size;
  source->is_fetching = false;
  source->next_offset = 0;
  sources_.emplace_back(source);
}

Option<FeedEntry> RemoteFeedReader::fetchNextEntry() {
  ScopedLock<std::mutex> lk(mutex_);

  for (const auto& source : sources_) {
    maybeFillBuffer(source.get());
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
      (int) source->max_buffer_size);

  rpc_client_->call(source->rpc_url, rpc.get());

  rpc->onSuccess([this, source] (const decltype(rpc)::ValueType& r) mutable {
    ScopedLock<std::mutex> lk(mutex_);
    source->is_fetching = false;
    lk.unlock();
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

void RemoteFeedReader::exportStats(
    const String& path_prefix /* = "/fnord/feeds/reader/" */,
    stats::StatsRepository* stats_repo /* = nullptr */) {
}

}
}

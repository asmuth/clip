/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/logging.h"
#include "fnord/json/JSONRPCCodec.h"
#include "fnord-feeds/FeedService.h"
#include "fnord-feeds/RemoteFeedWriter.h"

namespace fnord {
namespace feeds {

RemoteFeedWriter::RemoteFeedWriter(
    RPCClient* rpc_client,
    size_t max_buffer_size /* = kDefaultBufferSize */) :
    rpc_client_(rpc_client),
    max_buffer_size_(max_buffer_size),
    sequence_(0) {}

void RemoteFeedWriter::appendEntry(const String& entry_data) {
  ScopedLock<std::mutex> lk(write_queue_mutex_);

  if (write_queue_.size() > max_buffer_size_) {
    RAISE(kBufferOverflowError, "feed writer buffer is full");
  }

  write_queue_.emplace_front(entry_data);
  lk.unlock();

  stat_entries_written_total_.incr(1);
  flushBuffer();
}

void RemoteFeedWriter::addTargetFeed(
    URI url,
    String feed_name,
    unsigned max_concurrent_requests) {
  auto target = new TargetFeed;
  target->rpc_url = url;
  target->feed_name = feed_name;
  target->max_concurrent_requests = max_concurrent_requests;
  target->cur_requests = 0;

  ScopedLock<std::mutex> lk(target_feeds_mutex_);
  target_feeds_.emplace_back(target);
}

void RemoteFeedWriter::flushBuffer() {
  ScopedLock<std::mutex> lk(target_feeds_mutex_);

  if (target_feeds_.empty()) {
    return;
  }

  for (int i = 0; i < target_feeds_.size(); ++i) {
    auto idx = ++sequence_ % target_feeds_.size();

    if (target_feeds_[idx]->cur_requests <
        target_feeds_[idx]->max_concurrent_requests) {
      auto feed = target_feeds_[idx];
      lk.unlock();
      flushBuffer(feed);
      return;
    }
  }

}

void RemoteFeedWriter::flushBuffer(RefPtr<TargetFeed> target) {
#ifndef FNORD_NOTRACE
    fnord::logTrace(
        "fnord.feeds.remotefeedwriter",
        "Trying to write to feed name=$0 url=$1",
        target->feed_name,
        target->rpc_url.toString());
#endif

  ScopedLock<std::mutex> lk(write_queue_mutex_);
  if (write_queue_.empty()) {
    return;
  }

  auto entry = write_queue_.back();
  write_queue_.pop_back();
  lk.unlock();

  auto rpc = fnord::mkRPC<json::JSONRPCCodec>(
      &FeedService::append,
      target->feed_name,
      entry);

  target->cur_requests++;
  rpc_client_->call(target->rpc_url, rpc.get());

  rpc->onSuccess([this, target] (const decltype(rpc)::ValueType& r) mutable {
    target->cur_requests--;
    stat_entries_written_success_.incr(1);
    flushBuffer();
  });

  rpc->onError([this, target, entry] (const Status& status) {
    target->cur_requests--;
    stat_entries_written_error_.incr(1);

    fnord::logError(
        "fnord.feeds.remotefeedwriter",
        "Writing to feed feed name=$0 url=$1 failed: $2",
        target->feed_name,
        target->rpc_url.toString(),
        status);

    ScopedLock<std::mutex> lk(write_queue_mutex_);
    write_queue_.emplace_back(entry);
    lk.unlock();
    stat_entries_written_retry_.incr(1);
    flushBuffer();
  });
}

void RemoteFeedWriter::exportStats(
    const String& path_prefix /* = "/fnord/feeds/writer/" */,
    stats::StatsRepository* stats_repo /* = nullptr */) {
  if (stats_repo == nullptr) {
    stats_repo = stats::StatsRepository::get();
  }

  stats_repo->exportStat(
      FileUtil::joinPaths(path_prefix, "entries_written_total"),
      &stat_entries_written_total_,
      stats::ExportMode::EXPORT_DELTA);

  stats_repo->exportStat(
      FileUtil::joinPaths(path_prefix, "entries_written_success"),
      &stat_entries_written_success_,
      stats::ExportMode::EXPORT_DELTA);

  stats_repo->exportStat(
      FileUtil::joinPaths(path_prefix, "entries_written_error"),
      &stat_entries_written_error_,
      stats::ExportMode::EXPORT_DELTA);

  stats_repo->exportStat(
      FileUtil::joinPaths(path_prefix, "entries_written_retry"),
      &stat_entries_written_retry_,
      stats::ExportMode::EXPORT_DELTA);
}


}
}

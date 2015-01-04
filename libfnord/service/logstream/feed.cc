/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/service/logstream/logstreamservice.h"
#include "fnord/service/logstream/feed.h"

namespace fnord {
namespace logstream_service {

LogStreamServiceFeed::LogStreamServiceFeed(
    const String& name,
    fnord::comm::RPCChannel* rpc_channel,
    int batch_size /* = kDefaultBatchSize */,
    int buffer_size /* = kDefaultBufferSize */) :
    fnord::comm::Feed(name),
    rpc_channel_(rpc_channel),
    batch_size_(batch_size),
    buffer_size_(buffer_size),
    offset_(0) {}

Future<bool> LogStreamServiceFeed::appendEntry(const String& entry) {
  Promise<bool> promise;

  auto rpc = fnord::comm::mkRPC(
      &LogStreamService::append,
      name(),
      entry);

  rpc->call(rpc_channel_);

  rpc->onSuccess([promise] (const decltype(rpc)::ValueType& r) mutable {
    promise.success(true);
  });

  rpc->onError([promise] (const Status& status) mutable {
    promise.failure(status);
  });

  return promise.future();
}

/*
void LogStreamServiceFeed::insertDone() {
  std::unique_lock<std::mutex> lk(insert_mutex_);
  cur_insert_rpc_.reset(nullptr);

  if (insert_buf_.size() > 0) {
    // FIXPAUL: batch insert
    cur_insert_rpc_ = fnord::comm::mkRPC(
        &LogStreamService::append,
        name(),
        insert_buf_.back());
    insert_buf_.pop_back();

    cur_insert_rpc_->onReady(
        std::bind(&LogStreamServiceFeed::insertDone, this));
    cur_insert_rpc_->call(rpc_channel_);
  }
}

void LogStreamServiceFeed::maybeFillBuffer() {
  if (cur_fetch_rpc_.get() != nullptr || fetch_buf_.size() >= buffer_size_) {
    return;
  }

  cur_fetch_rpc_ = fnord::comm::mkRPC(
      &LogStreamService::fetch,
      name(),
      offset_,
      batch_size_);

  cur_fetch_rpc_->call(rpc_channel_);
}

void LogStreamServiceFeed::fillBuffer() {
  maybeFillBuffer();
  cur_fetch_rpc_->wait();

  for (const auto& entry : cur_fetch_rpc_->result()) {
    fetch_buf_.emplace_back(entry);
    offset_ = entry.next_offset;
  }

  cur_fetch_rpc_.reset(nullptr);
}

bool LogStreamServiceFeed::getNextEntry(String* entry) {
  std::unique_lock<std::mutex> lk(fetch_mutex_);

  if (fetch_buf_.empty()) {
    fillBuffer();
  } else {
    maybeFillBuffer();
  }

  if (fetch_buf_.empty()) {
    return false;
  }

  *entry = fetch_buf_.front().data;
  fetch_buf_.pop_front();
  return true;
}
*/

void LogStreamServiceFeed::setOption(
    const String& optname,
    const String& optval) {
  if (optname == "batch_size") {
    batch_size_ = std::stoi(optval);
    return;
  }

  if (optname == "buffer_size") {
    buffer_size_ = std::stoi(optval);
    return;
  }

  RAISEF(kInvalidOptionError, "invalid option: $0", optname);
}

}
}

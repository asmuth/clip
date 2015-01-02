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
    const std::string& name,
    fnord::comm::RPCChannel* rpc_channel,
    int batch_size /* = kDefaultBatchSize */,
    int buffer_size /* = kDefaultBufferSize */) :
    fnord::comm::Feed(name),
    rpc_channel_(rpc_channel),
    batch_size_(batch_size),
    buffer_size_(buffer_size),
    offset_(0) {}

void LogStreamServiceFeed::append(const std::string& entry) {
  auto rpc = fnord::comm::mkRPC(&LogStreamService::append, name(), entry);
  rpc->call(rpc_channel_);
  comm::AnyRPC::fireAndForget(std::move(rpc));
}

void LogStreamServiceFeed::maybeFillBuffer() {
  if (cur_rpc_.get() != nullptr || buf_.size() >= buffer_size_) {
    return;
  }

  cur_rpc_ = fnord::comm::mkRPC(
      &LogStreamService::fetch,
      name(),
      offset_,
      batch_size_);

  cur_rpc_->call(rpc_channel_);
}

void LogStreamServiceFeed::fillBuffer() {
  maybeFillBuffer();
  cur_rpc_->wait();

  for (const auto& entry : cur_rpc_->result()) {
    buf_.emplace_back(entry);
    offset_ = entry.next_offset;
  }

  cur_rpc_.reset(nullptr);
}

bool LogStreamServiceFeed::getNextEntry(std::string* entry) {
  if (buf_.empty()) {
    fillBuffer();
  } else {
    maybeFillBuffer();
  }

  if (buf_.empty()) {
    return false;
  }

  *entry = buf_.front().data;
  buf_.pop_front();
  return true;
}

void LogStreamServiceFeed::setOption(
    const std::string& optname,
    const std::string& optval) {
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

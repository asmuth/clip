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
    int batch_size /* = kDefaultBatchSize */) :
    fnord::comm::Feed(name),
    rpc_channel_(rpc_channel),
    batch_size_(batch_size),
    offset_(0) {}

void LogStreamServiceFeed::append(const std::string& entry) {
  auto rpc = fnord::comm::mkRPC(&LogStreamService::append, name(), entry);
  rpc->call(rpc_channel_);
  comm::AnyRPC::fireAndForget(std::move(rpc));
}

void LogStreamServiceFeed::fillBuffer() {
  auto rpc = fnord::comm::mkRPC(
      &LogStreamService::fetch,
      name(),
      offset_,
      batch_size_);

  rpc->call(rpc_channel_);
  rpc->wait();

  for (const auto& entry : rpc->result()) {
    buf_.emplace_back(entry);
    offset_ = entry.next_offset;
  }
}

bool LogStreamServiceFeed::getNextEntry(std::string* entry) {
  if (buf_.empty()) {
    fillBuffer();
  }

  if (buf_.empty()) {
    return false;
  }

  *entry = buf_.front().data;
  buf_.pop_front();
  return true;
}

}
}

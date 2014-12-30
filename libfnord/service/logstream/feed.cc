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
    fnord::comm::RPCChannel* rpc_channel) :
    fnord::comm::Feed(name),
    rpc_channel_(rpc_channel) {}

void LogStreamServiceFeed::append(const std::string& entry) {
  auto rpc = fnord::comm::mkRPC(&LogStreamService::append, name(), entry);
  rpc->call(rpc_channel_);
  comm::AnyRPC::fireAndForget(std::move(rpc));
}

}
}

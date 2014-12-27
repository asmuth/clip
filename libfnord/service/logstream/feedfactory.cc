/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/service/logstream/feed.h"
#include "fnord/service/logstream/feedfactory.h"

namespace fnord {
namespace logstream_service {

LogStreamServiceFeedFactory::LogStreamServiceFeedFactory(
    comm::RPCChannel* rpc_channel) :
    rpc_channel_(rpc_channel) {}

std::unique_ptr<fnord::comm::Feed> LogStreamServiceFeedFactory::getFeed(
    const std::string& name) {
  return std::unique_ptr<fnord::comm::Feed>(
      new fnord::logstream_service::LogStreamServiceFeed(name, rpc_channel_));
}

}
}

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOGSTREAM_SERVICE_FEEDFACTORY_H
#define _FNORD_LOGSTREAM_SERVICE_FEEDFACTORY_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "fnord/comm/feed.h"
#include "fnord/comm/rpc.h"

namespace fnord {
namespace logstream_service {

class LogStreamServiceFeedFactory : public fnord::comm::FeedFactory {
public:
  LogStreamServiceFeedFactory(comm::RPCChannel* rpc_channel);
  std::unique_ptr<fnord::comm::Feed> getFeed(const std::string& name) override;
protected:
  comm::RPCChannel* rpc_channel_;
};

}
}
#endif

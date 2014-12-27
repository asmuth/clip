/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOGSTREAM_SERVICE_FEED_H
#define _FNORD_LOGSTREAM_SERVICE_FEED_H
#include "fnord/comm/feed.h"
#include "fnord/comm/rpc.h"

namespace fnord {
namespace logstream_service {

class LogStreamServiceFeed : public fnord::comm::Feed {
public:
  LogStreamServiceFeed(
      const std::string& name,
      fnord::comm::RPCChannel* rpc_channel);

  uint64_t append(const std::string& entry) override;

protected:
  fnord::comm::RPCChannel* rpc_channel_;
};

}
}
#endif

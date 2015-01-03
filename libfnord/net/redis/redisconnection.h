/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_REDIS_CONNECTION_H
#define _FNORD_REDIS_CONNECTION_H
#include <memory>
#include <vector>
#include <fnord/3rdparty/hiredis/hiredis.h>
#include "fnord/base/option.h"
#include "fnord/base/status.h"
#include <fnord/logging/logger.h>
#include <fnord/net/inetaddr.h>

namespace fnord {
namespace redis {

class RedisConnection {
public:
  RedisConnection(
      const fnord::net::InetAddr& addr,
      fnord::thread::TaskScheduler* scheduler);

  ~RedisConnection();

  void executeCommand(
      const std::vector<std::string>& args,
      std::function<void (
          const Status& status,
          const Option<std::string>& reply)> callback);

protected:
  redisContext* ctx_;
};

}
}
#endif

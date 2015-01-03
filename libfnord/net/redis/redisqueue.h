/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_REDIS_QUEUE_H
#define _FNORD_REDIS_QUEUE_H
#include <memory>
#include <vector>
#include <fnord/net/redis/redisconnection.h>

namespace fnord {
namespace redis {

class RedisQueue {
public:
  RedisQueue(
      const std::string& key_prefix,
      std::unique_ptr<RedisConnection> conn);

protected:
  std::string key_prefix_;
  std::unique_ptr<RedisConnection> conn_;
};

}
}
#endif

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
#include <functional>
#include <memory>
#include <vector>
#include <fnord-base/net/redis/redisconnection.h>
#include <fnord/comm/queue.h>

namespace fnord {
namespace redis {

class RedisQueue : public comm::Queue {
public:
  RedisQueue(
      const std::string& redis_key,
      std::unique_ptr<RedisConnection> conn);

  Future<bool> enqueueJob(const QueueJob& job) override;

  Future<QueueJob> leaseJob() override;
  Future<Option<QueueJob>> maybeLeaseJob() override;

  Future<bool> commitJob(const QueueJob& job, const Status& status) override;

  void setOption(
      const std::string& optname,
      const std::string& optval) override;

protected:
  std::string redis_key_;
  std::unique_ptr<RedisConnection> conn_;
};

}
}
#endif

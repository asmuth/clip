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
#include <fnord/comm/queue.h>

namespace fnord {
namespace redis {

class RedisQueue : public comm::Queue {
public:
  RedisQueue(
      const std::string& redis_key,
      std::unique_ptr<RedisConnection> conn);

  void enqueueJob(const QueueJob& job) override;

  void enqueueJobAsync(
      const QueueJob& job,
      std::function<void (const Status& status)> callback) override;

  void enqueueJobAsyncUnsafe(const QueueJob& job) override;

  QueueJob leaseJob() override;

  void leaseJobAsync(std::function<void (const QueueJob& job)>) override;

  Option<QueueJob> maybeLeaseJob() override;

  void maybeLeaseJobAsync(
      std::function<void (const Option<QueueJob>& job)>) override;

  void commitJobSuccess(const QueueJob& job);

  void commitJobSuccessAsync(
      const QueueJob& job,
      std::function<void (const Status& status)> callback) override;

  void commitJobSuccessAsyncUnsafe(const QueueJob& job) override;

  void commitJobError(
      const QueueJob& job,
      const std::exception& error) override;

  void commitJobErrorAsync(
      const QueueJob& job,
      std::function<void (const Status& status)> callback) override;

  void commitJobErrorAsyncUnsafe(const QueueJob& job) override;

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

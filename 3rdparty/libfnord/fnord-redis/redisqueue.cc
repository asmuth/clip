/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/net/redis/redisqueue.h>

namespace fnord {
namespace redis {

RedisQueue::RedisQueue(
    const std::string& redis_key,
    std::unique_ptr<RedisConnection> conn) :
    comm::Queue(redis_key),
    redis_key_(redis_key),
    conn_(std::move(conn)) {}

Future<bool> RedisQueue::enqueueJob(const QueueJob& job) {
  RAISE(kNotYetImplementedError);
}

Future<comm::Queue::QueueJob> RedisQueue::leaseJob() {
  Promise<QueueJob> promise;

  auto reply = conn_->blpop(redis_key_, 0);

  reply.onSuccess([promise] (const std::vector<std::string>& r) mutable {
    if (r.size() == 2) {
      QueueJob job;
      job.job_data = r[1];
      promise.success(job);
    } else {
      promise.failure(Status(eRuntimeError, "invalid blpop reply"));
    }
  });

  reply.onFailure([promise] (const Status& status) mutable {
    promise.failure(status);
  });

  return promise.future();
}

Future<Option<comm::Queue::QueueJob>> RedisQueue::maybeLeaseJob() {
  Promise<Option<QueueJob>> promise;

  auto reply = conn_->lpop(redis_key_);

  reply.onSuccess([promise] (const Option<std::string>& reply) mutable {
    if (reply.isEmpty()) {
      promise.success(None<QueueJob>());
    } else {
      QueueJob job;
      job.job_data = reply.get();
      promise.success(Some(job));
    }
  });

  reply.onFailure([promise] (const Status& status) mutable {
    promise.failure(status);
  });

  return promise.future();
}

Future<bool> RedisQueue::commitJob(
    const QueueJob& job,
    const Status& error) {
  Promise<bool> promise;
  promise.success(true);
  return promise.future();
}

void RedisQueue::setOption(
    const std::string& optname,
    const std::string& optval) {
  RAISEF(kInvalidOptionError, "invalid option: $0", optname);
}

}
}

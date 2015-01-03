/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/net/redis/redisqueue.h>

namespace fnord {
namespace redis {

RedisQueue::RedisQueue(
    const std::string& redis_key,
    std::unique_ptr<RedisConnection> conn) :
    comm::Queue(redis_key),
    redis_key_(redis_key),
    conn_(std::move(conn)) {}

void RedisQueue::enqueueJob(const QueueJob& job) {
  RAISE(kNotYetImplementedError);
}

void RedisQueue::enqueueJobAsync(
    const QueueJob& job,
    std::function<void (const Status& status)> callback) {
  RAISE(kNotYetImplementedError);
}

void RedisQueue::enqueueJobAsyncUnsafe(const QueueJob& job) {
  RAISE(kNotYetImplementedError);
}

comm::Queue::QueueJob RedisQueue::leaseJob() {
  thread::Wakeup wakeup;
  Status status(eSuccess);
  QueueJob job;

  conn_->blpop(redis_key_, 0, [&wakeup, &status, &job] (
      const Status& s,
      const Option<std::vector<std::string>>& reply) {
    if (s.isSuccess()) {
      if (reply.get().size() == 2) {
        job.job_data = reply.get()[1];
      } else {
        status = Status(eRuntimeError, "invalid blpop reply");
      }
    } else {
      status = s;
    }

    wakeup.wakeup();
  });

  wakeup.waitForFirstWakeup();
  status.raiseIfError();
  return job;
}

void RedisQueue::leaseJobAsync(
    std::function<void (const Status& status, const QueueJob& job)>) {
  RAISE(kNotYetImplementedError);
}

Option<comm::Queue::QueueJob> RedisQueue::maybeLeaseJob() {
  thread::Wakeup wakeup;
  Status status(eSuccess);
  Option<QueueJob> job;

  conn_->lpop(redis_key_, [&wakeup, &status, &job] (
      const Status& s,
      const Option<std::string>& reply) {
    status = s;

    if (s.isSuccess() && !reply.isEmpty()) {
      QueueJob j;
      j.job_data = reply.get();
      job = j;
    }

    wakeup.wakeup();
  });

  wakeup.waitForFirstWakeup();
  status.raiseIfError();
  return job;
}

void RedisQueue::maybeLeaseJobAsync(
    std::function<void (const Status& status, const Option<QueueJob>& job)>) {
  RAISE(kNotYetImplementedError)
}

void RedisQueue::commitJobSuccess(const QueueJob& job) {
  /* noop */
}

void RedisQueue::commitJobSuccessAsync(
    const QueueJob& job,
    std::function<void (const Status& status)> callback) {
  /* noop */
}

void RedisQueue::commitJobSuccessAsyncUnsafe(const QueueJob& job) {
  /* noop */
}

void RedisQueue::commitJobError(
    const QueueJob& job,
    const std::exception& error) {
  /* noop */
}

void RedisQueue::commitJobErrorAsync(
    const QueueJob& job,
    std::function<void (const Status& status)> callback) {
  /* noop */
}

void RedisQueue::commitJobErrorAsyncUnsafe(const QueueJob& job) {
  /* noop */
}

void RedisQueue::setOption(
    const std::string& optname,
    const std::string& optval) {
  RAISEF(kInvalidOptionError, "invalid option: $0", optname);
}

}
}

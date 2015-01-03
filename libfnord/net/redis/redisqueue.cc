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
    const std::string& key_prefix,
    std::unique_ptr<RedisConnection> conn) :
    comm::Queue(key_prefix),
    key_prefix_(key_prefix),
    conn_(std::move(conn)) {}

void RedisQueue::enqueueJob(const QueueJob& job) {}

void RedisQueue::enqueueJobAsync(
    const QueueJob& job,
    std::function<void (const Status& status)> callback) {}

void RedisQueue::enqueueJobAsyncUnsafe(const QueueJob& job) {}

comm::Queue::QueueJob RedisQueue::leaseJob() {}

void RedisQueue::leaseJobAsync(std::function<void (const QueueJob& job)>) {}

Option<comm::Queue::QueueJob> RedisQueue::maybeLeaseJob() {}

void RedisQueue::maybeLeaseJobAsync(
    std::function<void (const Option<QueueJob>& job)>) {}

void RedisQueue::commitJobSuccess(const QueueJob& job) {}

void RedisQueue::commitJobSuccessAsync(
    const QueueJob& job,
    std::function<void (const Status& status)> callback) {}

void RedisQueue::commitJobSuccessAsyncUnsafe(const QueueJob& job) {}

void RedisQueue::commitJobError(
    const QueueJob& job,
    const std::exception& error) {}

void RedisQueue::commitJobErrorAsync(
    const QueueJob& job,
    std::function<void (const Status& status)> callback) {}

void RedisQueue::commitJobErrorAsyncUnsafe(const QueueJob& job) {}

void RedisQueue::setOption(
    const std::string& optname,
    const std::string& optval) {}

}
}

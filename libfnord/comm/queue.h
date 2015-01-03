/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_COMM_QUEUE_H
#define _FNORD_COMM_QUEUE_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/base/option.h"
#include "fnord/base/status.h"

namespace fnord {
namespace comm {

class Queue {
public:
  struct QueueJob {
    std::string job_data;
    std::string job_id;
    int job_priority;
  };

  Queue(const std::string& name);
  virtual ~Queue();

  const std::string& name() const;

  /**
   * Insert a job into the queue and wait until the job is commited or raise
   * an exception
   */
  virtual void enqueueJob(const QueueJob& job) = 0;

  /**
   * Insert a job into the queue and return immediately
   */
  virtual void enqueueJobAsync(
      const QueueJob& job,
      std::function<void (const Status& status)> callback) = 0;

  /**
   * Insert a job into the queue and return immediately, don't care about
   * errors
   */
  virtual void enqueueJobAsyncUnsafe(const QueueJob& job) = 0;

  /**
   * Lease a job from the queue, block until a job is available
   */
  virtual QueueJob leaseJob() = 0;

  /**
   * Lease a job from the queue, execute the callback as soon as a job is
   * available
   */
  virtual void leaseJobAsync(std::function<void (const QueueJob& job)>) = 0;

  /**
   * Lease a job from the queue if one is available, blocking call
   */
  virtual Option<QueueJob> maybeLeaseJob() = 0;

  /**
   * lease a job from the queue, call the callback when a job is available
   * The contents of job are undefined if is_valid is false
   */
  virtual void maybeLeaseJobAsync(
      std::function<void (const Option<QueueJob>& job)>) = 0;

  /**
   * Commit a job as successfully processed, blcoking call
   */
  virtual void commitJobSuccess(const QueueJob& job);

  /**
   * Commit a job as successfully processed, non blocking call
   */
  virtual void commitJobSuccessAsync(
      const QueueJob& job,
      std::function<void (const Status& status)> callback) = 0;

  /**
   * Commit a job as successfully processed, non blocking call will swallow
   * exceptions (uses a fire and forget rpc)
   */
  virtual void commitJobSuccessAsyncUnsafe(const QueueJob& job) = 0;

  /**
   * Commit a job as error, blocking call
   */
  virtual void commitJobError(
      const QueueJob& job,
      const std::exception& error);

  /**
   * Commit a job as error, non blocking call
   */
  virtual void commitJobErrorAsync(
      const QueueJob& job,
      std::function<void (const Status& status)> callback) = 0;

  /**
   * Commit a job as error, non blocking call will swallow exceptions (uses a
   * fire and forget rpc)
   */
  virtual void commitJobErrorAsyncUnsafe(const QueueJob& job) = 0;

  virtual void setOption(
      const std::string& optname,
      const std::string& optval) = 0;

protected:
  std::string name_;
};

class QueueFactory {
public:
  virtual ~QueueFactory() {}
  virtual std::unique_ptr<Queue> getQueue(const std::string& name) = 0;
};

}
}
#endif

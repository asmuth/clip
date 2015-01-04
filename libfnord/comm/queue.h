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
#include <fnord/thread/future.h>

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
   * Insert a job into the queue
   */
  virtual Future<bool> enqueueJob(const QueueJob& job) = 0;

  /**
   * Lease a job from the queue
   */
  virtual Future<QueueJob> leaseJob() = 0;

  /**
   * Lease a job from the queue if one is available
   */
  virtual Future<Option<QueueJob>> maybeLeaseJob() = 0;

  /**
   * Commit a job as finish
   */
  virtual Future<bool> commitJob(const QueueJob& job, const Status& status) = 0;

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

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/task.h>
#include <metrictools/util/time.h>
#include <metrictools/util/logging.h>

namespace fnordmetric {

PeriodicTask::PeriodicTask(
    uint64_t interval) :
    interval_(interval),
    running_(false) {}

void PeriodicTask::start() {
  uint64_t last_run = 0;

  std::unique_lock<std::mutex> lk(mutex_);
  running_ = true;
  while (running_) {
    auto now = MonotonicClock::now();
    if (now - last_run < interval_) {
      cv_.wait_for(lk, std::chrono::microseconds(interval_ - (now - last_run)));
    }

    if (!running_) {
      break;
    }

    last_run = now;
    lk.unlock();

    auto rc = ReturnCode::success();
    auto t0 = MonotonicClock::now();
    try {
      rc = invoke();
    } catch (const std::exception& e) {
      rc = ReturnCode::error("ERUNTIME", e.what());
    }

    auto t1 = MonotonicClock::now();

    if (!rc.isSuccess()) {
      logWarning("Task failed: $0", rc.getMessage());
    }

    logDebug("Task took $0ms", double(t1 - t0) / 1000.0f);
    lk.lock();
  }
}

void PeriodicTask::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  running_ = false;
  cv_.notify_all();
}

void TaskRunner::addTask(std::unique_ptr<Task> task) {
  std::unique_lock<std::mutex> lk(mutex_);
  tasks_.emplace_back(std::move(task));
}

ReturnCode TaskRunner::runOnce() {
  return ReturnCode::success();
}

void TaskRunner::start() {
  for (auto& t : tasks_) {
    threads_.emplace_back(
        std::thread(std::bind(&Task::start, t.get())));
  }
}

void TaskRunner::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  for (auto& t : tasks_) {
    t->shutdown();
  }

  for (auto& t : threads_) {
    t.join();
  }
}

} // namespace fnordmetric


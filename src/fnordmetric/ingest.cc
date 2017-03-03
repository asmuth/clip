/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ingest.h>
#include <fnordmetric/util/time.h>
#include <fnordmetric/util/logging.h>

namespace fnordmetric {

IngestionTaskConfig::IngestionTaskConfig() :
    metric_id_rewrite_enabled(false) {}

/*
ReturnCode mkIngestionTask(
    MetricService* metric_service,
    const IngestionTaskConfig* sensor_cfg,
    std::unique_ptr<IngestionTask>* sensor_task) {
  {
    auto cfg = dynamic_cast<const HTTPIngestionTaskConfig*>(sensor_cfg);
    if (cfg) {
      sensor_task->reset(new HTTPIngestionTask(cfg, metric_service));
      return ReturnCode::success();
    }
  }

  return ReturnCode::errorf(
      "EARG",
      "invalid sensor type for sensor: $0",
      sensor_cfg->sensor_id);
}
*/

SensorScheduler::SensorScheduler(
    size_t thread_count /* = 1 */) :
    thread_count_(thread_count),
    running_(false),
    queue_([] (const IngestionTask* a, const IngestionTask* b) {
      return a->getNextInvocationTime() < b->getNextInvocationTime();
    }) {}

void SensorScheduler::addTask(std::unique_ptr<IngestionTask> task) {
  std::unique_lock<std::mutex> lk(mutex_);
  queue_.emplace(task.get());
  tasks_.emplace_back(std::move(task));
  cv_.notify_all();
}

ReturnCode SensorScheduler::executeNextTask() {
  std::unique_lock<std::mutex> lk(mutex_);

  if (queue_.size() == 0) {
    cv_.wait(lk);
    return ReturnCode::success();
  }

  IngestionTask* task;
  while (running_) {
    if (queue_.empty()) {
      cv_.wait(lk);
      continue;
    }

    auto now = MonotonicClock::now();
    auto next_task = (*queue_.begin())->getNextInvocationTime();
    if (now < next_task) {
      cv_.wait_for(
          lk,
          std::chrono::microseconds(next_task - now));

      continue;
    }

    task = *queue_.begin();
    queue_.erase(queue_.begin());
    break;
  }

  cv_.notify_all();
  lk.unlock();

  if (!running_) {
    return ReturnCode::success();
  }

  auto rc = ReturnCode::success();
  try {
    rc = task->invoke();
  } catch (const std::exception& e) {
    rc = ReturnCode::error("ERUNTIME", e.what());
  }

  lk.lock();
  queue_.emplace(task);
  cv_.notify_all();

  return rc;
}

ReturnCode SensorScheduler::start() {
  running_ = true;
  for (size_t i = 0; i < thread_count_; ++i) {
    threads_.emplace_back(std::thread([this] {
      while (running_) {
        auto rc = executeNextTask();
        if (!rc.isSuccess()) {
          logError("Sensor task failed: $0", rc.getMessage());
        }
      }
    }));
  }

  return ReturnCode::success();
}

void SensorScheduler::shutdown() {
  std::unique_lock<std::mutex> lk(mutex_);
  running_ = false;
  cv_.notify_all();
  lk.unlock();

  for (auto& t : threads_) {
    t.join();
  }
}

} // namespace fnordmetric


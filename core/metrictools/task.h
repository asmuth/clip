/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <list>
#include <thread>
#include <regex>
#include <metrictools/util/return_code.h>

namespace fnordmetric {
class Task {
public:

  virtual ~Task() = default;

  virtual void start() = 0;
  virtual void shutdown() = 0;

};

class PeriodicTask : public Task {
public:

  PeriodicTask(uint64_t interval);

  virtual ReturnCode invoke() = 0;

  void start() override;
  void shutdown() override;

protected:
  uint64_t interval_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool running_;
};

class TaskRunner {
public:

  void addTask(std::unique_ptr<Task> task);

  ReturnCode runOnce();

  void start();
  void shutdown();

protected:
  std::mutex mutex_;
  std::vector<std::unique_ptr<Task>> tasks_;
  std::vector<std::thread> threads_;
};

} // namespace fnordmetric


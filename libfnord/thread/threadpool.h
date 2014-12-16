/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_THREAD_THREADPOOL_H
#define _FNORDMETRIC_THREAD_THREADPOOL_H
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include "fnord/thread/task.h"
#include "fnord/thread/taskscheduler.h"
#include "fnord/base/exceptionhandler.h"

namespace fnord {
namespace thread {

/**
 * A threadpool is threadsafe
 */
class ThreadPool : public TaskScheduler {
public:
  ThreadPool();

  ThreadPool(
      std::unique_ptr<fnord::ExceptionHandler> error_handler);

  void run(std::function<void()> task) override;
  void runOnReadable(std::function<void()> task, int fd) override;
  void runOnWritable(std::function<void()> task, int fd) override;

protected:
  void startThread();

  std::unique_ptr<fnord::ExceptionHandler> error_handler_;
  std::mutex runq_mutex_;
  std::list<std::function<void()>> runq_;
  std::condition_variable wakeup_;
  int free_threads_;
};

}
}
#endif

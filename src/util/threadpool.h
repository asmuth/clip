/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_THREADPOOL_H
#define _FNORDMETRIC_UTIL_THREADPOOL_H
#include <functional>
#include <atomic>
#include <fnordmetric/util/exceptionhandler.h>

namespace fnordmetric {
namespace util {

/**
 * A threadpool is threadsafe
 */
class ThreadPool {
public:
  ThreadPool(int max_threads, std::unique_ptr<ExceptionHandler> error_handler);

  /**
   * Run the provided lambda on the thread pool. Raises an exception if thread
   * pool is full
   */
  void run(std::function<void()> runnable);

protected:
  int max_threads_;
  std::unique_ptr<ExceptionHandler> error_handler_;
  std::atomic_int num_threads_;
};

}
}
#endif

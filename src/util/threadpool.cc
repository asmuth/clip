/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <thread>
#include <memory>
#include <fnordmetric/util/threadpool.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace util {

ThreadPool::ThreadPool(
    int max_threads,
    std::unique_ptr<ExceptionHandler> error_handler) :
    max_threads_(max_threads),
    error_handler_(std::move(error_handler)),
    num_threads_(0) {}

void ThreadPool::run(std::function<void()> runnable) {
  if (num_threads_++ >= max_threads_) {
    num_threads_--;
    RAISE(RuntimeException, "too many threads");
  }

  ThreadPool* self = this;
  new std::thread([self, runnable] () {
    try {
      runnable();
    } catch (const std::exception& e) {
      self->error_handler_->onException(e);
    }
    self->num_threads_--;
  });
}

}
}

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <memory>
#include <sys/select.h>
#include <thread>

#include "fnord/base/exception.h"
#include "fnord/base/exceptionhandler.h"
#include "fnord/thread/threadpool.h"

using fnord::ExceptionHandler;

namespace fnord {
namespace thread {

ThreadPool::ThreadPool() :
    ThreadPool(std::unique_ptr<fnord::ExceptionHandler>(
          new fnord::CatchAndPrintExceptionHandler(nullptr))) {}

ThreadPool::ThreadPool(
    std::unique_ptr<ExceptionHandler> error_handler) :
    error_handler_(std::move(error_handler)),
    free_threads_(0) {}

void ThreadPool::run(std::shared_ptr<Task> task) {
  runInternal([this, task] () {
    task->run();
  });
}

void ThreadPool::runOnReadable(std::shared_ptr<Task> task, int fd) {
  runInternal([this, task, fd] () {
    fd_set op_read, op_write;
    FD_ZERO(&op_read);
    FD_ZERO(&op_write);
    FD_SET(fd, &op_read);

    auto res = select(fd + 1, &op_read, &op_write, NULL, NULL);

    if (res == 0) {
      RAISE(kIOError, "unexpected timeout while select()ing");
    }

    if (res == -1) {
      RAISE_ERRNO(kIOError, "select() failed");
    }

    task->run();
  });
}

void ThreadPool::runOnWritable(std::shared_ptr<Task> task, int fd) {
  runInternal([task, fd] () {
    fd_set op_read, op_write;
    FD_ZERO(&op_read);
    FD_ZERO(&op_write);
    FD_SET(fd, &op_write);

    auto res = select(fd + 1, &op_read, &op_write, NULL, NULL);

    if (res == 0) {
      RAISE(kIOError, "unexpected timeout while select()ing");
    }

    if (res == -1) {
      RAISE_ERRNO(kIOError, "select() failed");
    }

    task->run();
  });
}

void ThreadPool::runInternal(std::function<void()> fn) {
  bool start_thread = true;

  {
    std::lock_guard<std::mutex> lock_holder(runq_mutex_);
    runq_.push_back(fn);

    if (runq_.size() < free_threads_) {
      start_thread = false;
    }
  }

  if (start_thread) {
    startThread();
  }

  wakeup_.notify_one();
}

void ThreadPool::startThread() {
  try {
    std::thread thread([this] () {
      for (;;) {
        try {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lk(runq_mutex_);
            free_threads_++;

            while (runq_.size() == 0) {
              wakeup_.wait(lk);
            }

            assert(runq_.size() > 0);
            task = runq_.front();
            runq_.pop_front();
            free_threads_--;
          }

          task();
        } catch (const std::exception& e) {
          this->error_handler_->onException(e);
        }
      }
    });

    thread.detach();
  } catch (const std::exception& e) {
    this->error_handler_->onException(e);
  }
}


}
}

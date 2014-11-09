/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <memory>
#include <fnordmetric/thread/threadpool.h>
#include <fnordmetric/util/runtimeexception.h>
#include <sys/select.h>
#include <thread>

using fnord::util::ExceptionHandler;

namespace fnord {
namespace thread {

ThreadPool::ThreadPool(
    std::unique_ptr<ExceptionHandler> error_handler) :
    error_handler_(std::move(error_handler)) {}

void ThreadPool::run(std::shared_ptr<Task> task) {
  new std::thread([this, task] () {
    try {
      task->run();
    } catch (const std::exception& e) {
      this->error_handler_->onException(e);
    }
  });
}

void ThreadPool::runOnReadable(std::shared_ptr<Task> task, int fd) {
  auto thread = new std::thread([this, task, fd] () {
    try {
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
    } catch (const std::exception& e) {
      this->error_handler_->onException(e);
    }
  });

  thread->detach();
}

void ThreadPool::runOnWritable(std::shared_ptr<Task> task, int fd) {
  new std::thread([this, task, fd] () {
    try {
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
    } catch (const std::exception& e) {
      this->error_handler_->onException(e);
    }
  });
}

}
}

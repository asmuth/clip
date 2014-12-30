/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "eventloop.h"
#include <fnord/base/exception.h>
#include <fnord/base/inspect.h>

namespace fnord {
namespace thread {

EventLoop::EventLoop() : max_fd_(1), running_(true) {
  callbacks_.reserve(FD_SETSIZE + 1);
  FD_ZERO(&op_read_);
  FD_ZERO(&op_write_);
}

void EventLoop::run(std::function<void()> task) {
  task(); // FIXPAUL post from different thread!!
}

void EventLoop::runOnReadable(std::function<void()> task, int fd) {
  if (fd > FD_SETSIZE) {
    RAISEF(kIOError, "fd is too large: $0, max is $1", fd, FD_SETSIZE);
  }

  if (fd > max_fd_) {
    max_fd_ = fd;
  }

  FD_SET(fd, &op_read_);
  FD_SET(fd, &op_error_);

  callbacks_[fd] = task;
}

void EventLoop::runOnWritable(std::function<void()> task, int fd) {
  if (fd > FD_SETSIZE) {
    RAISEF(kIOError, "fd is too large: $0, max is $1", fd, FD_SETSIZE);
  }

  if (fd > max_fd_) {
    max_fd_ = fd;
  }

  FD_SET(fd, &op_write_);
  FD_SET(fd, &op_error_);

  callbacks_[fd] = task;
}

void EventLoop::poll() {
  fd_set op_read, op_write, op_error;

  memcpy(&op_read, &op_read_, sizeof(fd_set));
  memcpy(&op_write, &op_write_, sizeof(fd_set));
  memcpy(&op_error, &op_error_, sizeof(fd_set));

  int res = select(max_fd_ + 1, &op_read, &op_write, &op_error, NULL);

  if (res == 0) {
    return;
  }

  if (res == -1) {
    RAISE_ERRNO(kIOError, "select() failed");
  }

  for (int fd = 0; fd <= max_fd_; fd++) {
    if (FD_ISSET(fd, &op_read)) {
      FD_CLR(fd, &op_read_);
      FD_CLR(fd, &op_error_);
      callbacks_[fd]();
      //callbacks_[fd] = nullptr;
    }

    else if (FD_ISSET(fd, &op_write)) {
      FD_CLR(fd, &op_write_);
      FD_CLR(fd, &op_error_);
      callbacks_[fd]();
      //callbacks_[fd] = nullptr;
    }
  }
}

void EventLoop::runOnWakeup(
    std::function<void()> task,
    Wakeup* wakeup,
    long wakeup_generation) {
  RAISE(kNotYetImplementedError);
}

void EventLoop::run() {
  while (running_) {
    poll();
  }
}

}
}

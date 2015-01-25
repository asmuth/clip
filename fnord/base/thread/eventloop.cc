/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <functional>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include "fnord/base/exception.h"
#include "fnord/base/inspect.h"
#include "fnord/base/thread/eventloop.h"

namespace fnord {
namespace thread {

EventLoop::EventLoop() :
    max_fd_(1),
    running_(true),
    threadid_(std::this_thread::get_id()),
    callbacks_(FD_SETSIZE + 1, nullptr) {
  FD_ZERO(&op_read_);
  FD_ZERO(&op_write_);
  FD_ZERO(&op_error_);
  setupRunQWakeupPipe();
}

EventLoop::~EventLoop() {
  close(runq_wakeup_pipe_[0]);
  close(runq_wakeup_pipe_[1]);
}

void EventLoop::setupRunQWakeupPipe() {
  if (pipe(runq_wakeup_pipe_) < 0) {
    RAISE_ERRNO(kIOError, "pipe() failed");
  }

  int flags = fcntl(runq_wakeup_pipe_[0], F_GETFL, 0);
  flags |= O_NONBLOCK;

  if (fcntl(runq_wakeup_pipe_[0], F_SETFL, flags) != 0) {
    RAISE_ERRNO(kIOError, "fnctl(%i) failed", runq_wakeup_pipe_[0]);
  }

  FD_SET(runq_wakeup_pipe_[0], &op_read_);
  if (runq_wakeup_pipe_[0] > max_fd_) {
    max_fd_ = runq_wakeup_pipe_[0];
  }

  callbacks_[runq_wakeup_pipe_[0]] = std::bind(&EventLoop::runQWakeup, this);
}

void EventLoop::run(std::function<void()> task) {
  if (std::this_thread::get_id() == threadid_) {
    task();
  } else {
    appendToRunQ(task);
  }
}

void EventLoop::appendToRunQ(std::function<void()> task) {
  std::unique_lock<std::mutex> lk(runq_mutex_);
  runq_.emplace_back(task);
  lk.unlock();
  write(runq_wakeup_pipe_[1], "\x0", 1);
}

void EventLoop::runOnReadable(std::function<void()> task, int fd) {
  if (std::this_thread::get_id() != threadid_) {
    appendToRunQ([this, task, fd] {
      runOnReadable(task, fd);
    });

    return;
  }

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
  if (std::this_thread::get_id() != threadid_) {
    appendToRunQ([this, task, fd] {
      runOnWritable(task, fd);
    });

    return;
  }

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
    if (errno == EINTR) {
      return;
    }

    RAISE_ERRNO(kIOError, "select() failed");
  }

  for (int fd = 0; fd <= max_fd_; fd++) {
    if (FD_ISSET(fd, &op_read)) {
      FD_CLR(fd, &op_read_);
      FD_CLR(fd, &op_error_);
      callbacks_[fd]();
    }

    else if (FD_ISSET(fd, &op_write)) {
      FD_CLR(fd, &op_write_);
      FD_CLR(fd, &op_error_);
      callbacks_[fd]();
    }
  }
}

void EventLoop::runOnWakeup(
    std::function<void()> task,
    Wakeup* wakeup,
    long wakeup_generation) {
  wakeup->onWakeup(wakeup_generation, [this, task] {
    this->run(task);
  });
}

void EventLoop::runQWakeup() {
  FD_SET(runq_wakeup_pipe_[0], &op_read_);

  static char devnull[512];
  while (read(runq_wakeup_pipe_[0], devnull, sizeof(devnull)) > 0);

  std::list<std::function<void()>> tasks;

  std::unique_lock<std::mutex> lk(runq_mutex_);
  tasks.splice(tasks.begin(), runq_);
  lk.unlock();

  for (const auto& task : tasks) {
    task();
  }
}

void EventLoop::run() {
  threadid_ = std::this_thread::get_id();

  while (running_) {
    poll();
  }
}

void EventLoop::shutdown() {
  running_ = false;
}

}
}

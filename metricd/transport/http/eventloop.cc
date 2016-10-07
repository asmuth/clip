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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "metricd/util/exception.h"
#include "metricd/transport/http/eventloop.h"

namespace fnordmetric {
namespace http {

EventLoop::EventLoop() :
    max_fd_(1),
    running_(true),
    threadid_(std::this_thread::get_id()),
    callbacks_(FD_SETSIZE + 1, nullptr),
    num_fds_(0) {
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
}

void EventLoop::runAsync(std::function<void()> task) {
   appendToRunQ(task);
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
  wakeup();
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
  ++num_fds_;

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
  ++num_fds_;

  callbacks_[fd] = task;
}

void EventLoop::cancelFD(int fd) {
  if (std::this_thread::get_id() != threadid_) {
    std::mutex m;
    bool done = false;
    std::condition_variable cv;

    appendToRunQ([this, fd, &cv, &m, &done] {
      cancelFD(fd);
      std::unique_lock<std::mutex> lk(m);
      done = true;
      cv.notify_all();
    });

    std::unique_lock<std::mutex> lk(m);
    while (!done) {
      cv.wait(lk);
    }

    return;
  }

  if (FD_ISSET(fd, &op_read_) || FD_ISSET(fd, &op_write_)) {
    --num_fds_;
  }

  FD_CLR(fd, &op_read_);
  FD_CLR(fd, &op_write_);
  FD_CLR(fd, &op_error_);
  callbacks_[fd] = nullptr;
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

  auto runq_fd = runq_wakeup_pipe_[0];
  if (FD_ISSET(runq_fd, &op_read)) {
    onRunQWakeup();
  }

  for (int fd = 0; fd <= max_fd_; fd++) {
    if (fd == runq_fd) {
      continue;
    }

    if (FD_ISSET(fd, &op_read)) {
      FD_CLR(fd, &op_read_);
      FD_CLR(fd, &op_error_);
      --num_fds_;

      if (callbacks_[fd]) {
        callbacks_[fd]();
      }
    }

    else if (FD_ISSET(fd, &op_write)) {
      FD_CLR(fd, &op_write_);
      FD_CLR(fd, &op_error_);
      --num_fds_;

      if (callbacks_[fd]) {
        callbacks_[fd]();
      }
    }
  }
}

void EventLoop::wakeup() {
  write(runq_wakeup_pipe_[1], "\x0", 1);
}

void EventLoop::runOnWakeup(
    std::function<void()> task,
    Wakeup* wakeup,
    long wakeup_generation) {
  wakeup->onWakeup(wakeup_generation, [this, task] {
    this->run(task);
  });
}

void EventLoop::onRunQWakeup() {
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

void EventLoop::runOnce() {
  threadid_ = std::this_thread::get_id();

  while (running_.load()) {
    {
      std::unique_lock<std::mutex> lk(runq_mutex_);
      if (runq_.empty() && num_fds_ == 0) {
        return;
      }
    }

    poll();
  }
}

void EventLoop::run() {
  threadid_ = std::this_thread::get_id();

  while (running_.load()) {
    poll();
  }
}

void EventLoop::shutdown() {
  usleep(10000); // FIXPAUL hack!!!!
  running_ = false;
  wakeup();
}

} // namespace http
} // namespace fnordmetric


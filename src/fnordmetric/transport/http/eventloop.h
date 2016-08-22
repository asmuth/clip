/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <list>
#include <sys/select.h>
#include <thread>
#include <vector>
#include <fnordmetric/transport/http/wakeup.h>

namespace fnordmetric {
namespace http {

class EventLoop {
public:
  void run(std::function<void()> task);
  void runAsync(std::function<void()> task);
  void runOnReadable(std::function<void()> task, int fd);
  void runOnWritable(std::function<void()> task, int fd);
  void runOnWakeup(
      std::function<void()> task,
      Wakeup* wakeup,
      long wakeup_generation);

  void cancelFD(int fd);

  EventLoop();
  ~EventLoop();
  void run();
  void runOnce();
  void shutdown();
  void wakeup();

protected:

  void poll();
  void setupRunQWakeupPipe();
  void onRunQWakeup();
  void appendToRunQ(std::function<void()> task);

  fd_set op_read_;
  fd_set op_write_;
  fd_set op_error_;
  int max_fd_;
  std::atomic<bool> running_;
  int runq_wakeup_pipe_[2];
  std::list<std::function<void()>> runq_;
  std::mutex runq_mutex_;
  std::thread::id threadid_;
  std::vector<std::function<void()>> callbacks_;
  size_t num_fds_;
};

} // namespace http
} // namespace fnordmetric


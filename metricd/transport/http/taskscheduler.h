/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_THREAD_TASKSCHEDULER_H
#define _libstx_THREAD_TASKSCHEDULER_H
#include <metricd/transport/http/task.h>
#include <metricd/transport/http/wakeup.h>

namespace fnordmetric {
namespace http {

class TaskScheduler {
public:
  virtual ~TaskScheduler() {}

  /**
   * Run the provided task as soon as possible
   */
  virtual void run(std::function<void()> task) = 0;

  /**
   * Run the provided task as soon as possible, but make sure not to run it
   * immediately from the same stack (to prevent deadlocks)
   */
  virtual void runAsync(std::function<void()> task) {
    run(task);
  }

  /**
   * Run the provided task when the provided filedescriptor becomes readable
   */
  virtual void runOnReadable(std::function<void()> task, int fd) = 0;

  /**
   * Run the provided task when the provided object becomes readable
   */
  template <class SelectableType>
  void runOnReadable(
      std::function<void()> task,
      const SelectableType& t) {
    runOnReadable(task, t.fd());
  }

  /**
   * Run the provided task when the provided filedescriptor becomes writable
   */
  virtual void runOnWritable(std::function<void()> task, int fd) = 0;

  /**
   * Run the provided task when the provided filedescriptor becomes writable
   */
  template <class SelectableType>
  void runOnWritable(
      std::function<void()> task,
      const SelectableType& t) {
    runOnWritable(task, t.fd());
  }

  /**
   * Run the provided task when the wakeup handle is woken up
   */
  void runOnNextWakeup(std::function<void()> task, Wakeup* wakeup) {
    runOnWakeup(task, wakeup, wakeup->generation());
  }

  /**
   * Run the provided task when the wakeup handle is woken up
   */
  void runOnFirstWakeup(std::function<void()> task, Wakeup* wakeup) {
    runOnWakeup(task, wakeup, 0);
  }

  /**
   * Run the provided task when the wakeup handle is woken up
   */
  virtual void runOnWakeup(
      std::function<void()> task,
      Wakeup* wakeup,
      long wakeup_generation) = 0;


  virtual void cancelFD(int fd) {};

};

}
}
#endif

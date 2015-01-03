/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_THREAD_TASKSCHEDULER_H
#define _FNORDMETRIC_THREAD_TASKSCHEDULER_H
#include "fnord/thread/task.h"
#include "fnord/thread/wakeup.h"

namespace fnord {
namespace thread {

class TaskScheduler {
public:
  virtual ~TaskScheduler() {}

  /**
   * Run the provided task as soon as possible
   */
  virtual void run(std::function<void()> task) = 0;

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
};

}
}
#endif

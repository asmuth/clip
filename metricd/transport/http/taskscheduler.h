/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
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

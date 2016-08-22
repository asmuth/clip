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
#ifndef _libstx_THREAD_TASK_H
#define _libstx_THREAD_TASK_H
#include <functional>
#include <memory>

namespace thread {

class Task {
public:
  virtual ~Task() {}

  template <class RunnableType>
  static std::shared_ptr<Task> create(RunnableType runnable);

  virtual void run() = 0;

};

template <class RunnableType>
class TaskImpl : public Task {
friend class Task;
public:

  void run() override;

protected:
  TaskImpl(RunnableType runnable) : runnable_(runnable) {}
  RunnableType runnable_;
};

template <class RunnableType>
std::shared_ptr<Task> Task::create(
    RunnableType runnable) {
  return std::shared_ptr<Task>(new TaskImpl<RunnableType>(runnable));
}

template <class RunnableType>
void TaskImpl<RunnableType>::run() {
  runnable_();
}

}
#endif

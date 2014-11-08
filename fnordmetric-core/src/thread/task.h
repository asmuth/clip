/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_THREAD_TASK_H
#define _FNORDMETRIC_THREAD_TASK_H
#include <functional>
#include <memory>

namespace fnord {
namespace thread {

class Task {
public:

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
}
#endif

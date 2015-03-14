/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_THREAD_QUEUE_H
#define _FNORDMETRIC_THREAD_QUEUE_H
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include "fnord-base/option.h"

namespace fnord {
namespace thread {

/**
 * A queue is threadsafe
 */
template <typename T>
class Queue {
public:

  void insert(const T& job);
  T pop();
  Option<T> poll();

protected:
  std::deque<T> queue_;
  std::mutex mutex_;
  std::condition_variable wakeup_;
};

}
}

#include "queue_impl.h"
#endif

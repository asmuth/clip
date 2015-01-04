/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_FUTURE_H
#define _FNORD_BASE_FUTURE_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include "fnord/base/inspect.h"
#include "fnord/base/exception.h"

namespace fnord {

template <typename T>
class PromiseState {
public:
  Wakeup wakeup;
  std::mutex mutex; // FIXPAUL use spinlock
  bool is_ready;
  char value_data_[sizeof(T)];
};

template <typename T>
class Future {
public:
  Future();
  Future(const Future<T>& other);
  Future(Future<T>&& other);
  ~Future();

  Future & operator=(const Future<T>& other);

  bool isError() const;
  bool isSuccess() const;

  void onError(std::function<void> fn);
  void onSuccess(std::function<void> fn);

  void wait() const;
  void wait(const Duration& timeout) const;

  void onReady(std::function<void> fn);
  void onReady(thread::TaskScheduler* scheduler, std::function<void> fn);

  const T& get() const;

protected:

};

} // namespace fnord

#endif

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_THREAD_FUTURE_H
#define _FNORD_THREAD_FUTURE_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include "fnord/base/autoref.h"
#include "fnord/base/duration.h"
#include "fnord/base/exception.h"
#include "fnord/base/inspect.h"
#include "fnord/thread/wakeup.h"

namespace fnord {
class TaskScheduler;

template <typename T>
class PromiseState : public RefCounted {
public:
  PromiseState();
  ~PromiseState();

  Wakeup wakeup;
  Status status;
  std::mutex mutex; // FIXPAUL use spinlock
  char value_data[sizeof(T)];
  T* value;
  bool ready;

  std::function<void (const Status& status)> on_failure;
  std::function<void (const T& value)> on_success;
};

template <typename T>
class Future {
public:
  Future(AutoRef<PromiseState<T>> promise_state);
  Future(const Future<T>& other);
  Future(Future<T>&& other);
  ~Future();

  Future& operator=(const Future<T>& other);

  bool isFailure() const;
  bool isSuccess() const;

  void onFailure(std::function<void (const Status& status)> fn);
  void onSuccess(std::function<void (const T& value)> fn);

  void wait() const;
  void wait(const Duration& timeout) const;

  void onReady(std::function<void> fn);
  void onReady(TaskScheduler* scheduler, std::function<void> fn);

  const T& get() const;
  const T& waitAndGet() const;

  Wakeup* wakeup() const;

protected:
  AutoRef<PromiseState<T>> state_;
};

template <typename T>
class Promise {
public:
  Promise();
  Promise(const Promise<T>& other);
  Promise(Promise<T>&& other);
  ~Promise();

  void success(const T& value);
  void success(T&& value);
  void failure(const std::exception& e);
  void failure(const Status& e);

  Future<T> future() const;

  bool isFulfilled() const;

protected:
  AutoRef<PromiseState<T>> state_;
};

} // namespace fnord

#include "future_impl.h"
#endif

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_THREAD_FUTURE_IMPL_H
#define _FNORD_THREAD_FUTURE_IMPL_H
#include <assert.h>

namespace fnord {

template <typename T>
PromiseState<T>::PromiseState() :
    status(eSuccess),
    value(nullptr),
    on_failure(nullptr),
    on_success(nullptr),
    ready(false) {}

template <typename T>
PromiseState<T>::~PromiseState() {
  assert(ready == true);

  if (value != nullptr) {
    value->~T();
  }
}

template <typename T>
Future<T>::Future(AutoRef<PromiseState<T>> state) : state_(state) {}

template <typename T>
Future<T>::Future(const Future<T>& other) : state_(other.state_) {}

template <typename T>
Future<T>::Future(Future<T>&& other) : state_(std::move(other.state_)) {}

template <typename T>
Future<T>::~Future() {}

template <typename T>
Future<T>& Future<T>::operator=(const Future<T>& other) {
  state_ = other.state_;
}

template <typename T>
void Future<T>::wait() const {
  state_->wakeup.waitForFirstWakeup();
}

template <typename T>
void Future<T>::onFailure(std::function<void (const Status& status)> fn) {
  std::unique_lock<std::mutex> lk(state_->mutex);

  if (!state_->ready) {
    state_->on_failure = fn;
  } else if (state_->status.isError()) {
    fn(state_->status);
  }
}

template <typename T>
void Future<T>::onSuccess(std::function<void (const T& value)> fn) {
  std::unique_lock<std::mutex> lk(state_->mutex);

  if (!state_->ready) {
    state_->on_success = fn;
  } else if (state_->status.isSuccess()) {
    fn(*(state_->value));
  }
}

template <typename T>
const T& Future<T>::get() const {
  std::unique_lock<std::mutex> lk(state_->mutex);

  if (!state_->ready) {
    RAISE(kFutureError, "get() called on pending future");
  }

  state_->status.raiseIfError();
  return *state_->value;
}

template <typename T>
Wakeup* Future<T>::wakeup() const {
  return &state_->wakeup;
}

template <typename T>
const T& Future<T>::waitAndGet() const {
  wait();
  return get();
}

template <typename T>
Promise<T>::Promise() : state_(new PromiseState<T>()) {}

template <typename T>
Promise<T>::Promise(const Promise<T>& other) : state_(other.state_) {}

template <typename T>
Promise<T>::Promise(Promise<T>&& other) : state_(std::move(other.state_)) {}

template <typename T>
Promise<T>::~Promise() {}

template <typename T>
Future<T> Promise<T>::future() const {
  return Future<T>(state_);
}

template <typename T>
void Promise<T>::failure(const std::exception& e) {
  failure(Status(e));
}

template <typename T>
void Promise<T>::failure(const Status& status) {
  std::unique_lock<std::mutex> lk(state_->mutex);
  if (state_->ready) {
    RAISE(kFutureError, "promise was already fulfilled");
  }

  state_->status = status;
  state_->ready = true;
  lk.unlock();

  state_->wakeup.wakeup();

  if (state_->on_failure) {
    state_->on_failure(state_->status);
  }
}

template <typename T>
void Promise<T>::success(const T& value) {
  std::unique_lock<std::mutex> lk(state_->mutex);
  if (state_->ready) {
    RAISE(kFutureError, "promise was already fulfilled");
  }

  state_->value = new (state_->value_data) T(value);
  state_->ready = true;
  lk.unlock();

  state_->wakeup.wakeup();

  if (state_->on_success) {
    state_->on_success(*(state_->value));
  }
}

template <typename T>
void Promise<T>::success(T&& value) {
  std::unique_lock<std::mutex> lk(state_->mutex);
  if (state_->ready) {
    RAISE(kFutureError, "promise was already fulfilled");
  }

  state_->value = new (state_->value_data) T(std::move(value));
  state_->ready = true;
  lk.unlock();

  state_->wakeup.wakeup();

  if (state_->on_success) {
    state_->on_success(*(state_->value));
  }
}

template <typename T>
bool Promise<T>::isFulfilled() const {
  std::unique_lock<std::mutex> lk(state_->mutex);
  return state_->ready;
}

} // namespace fnord

#endif

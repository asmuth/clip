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

namespace fnord {

template <typename T>
PromiseState<T>::PromiseState() : status(eSuccess), value(nullptr) {}

template <typename T>
PromiseState<T>::~PromiseState() {
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

/*
  bool Future<T>::isFailure() const;
  bool Future<T>::isSuccess() const;

  void onFailure(std::function<void> fn);
  void onSuccess(std::function<void> fn);

  void wait(const Duration& timeout) const;

  void onReady(std::function<void> fn);
  void onReady(TaskScheduler* scheduler, std::function<void> fn);

*/
template <typename T>
const T& Future<T>::get() const {
  std::unique_lock<std::mutex> lk(state_->mutex);

  if (state_->wakeup.generation() != 1) {
    RAISE(kFutureError, "get() called on pending future");
  }

  state_->status.raiseIfError();
  return *state_->value;
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
void Promise<T>::failure(const Status& status) {
  std::unique_lock<std::mutex> lk(state_->mutex);
  state_->status = status;
  lk.unlock();
  state_->wakeup.wakeup();
}

template <typename T>
void Promise<T>::success(const T& value) {
  std::unique_lock<std::mutex> lk(state_->mutex);
  state_->value = new (state_->value_data) T(value);
  lk.unlock();
  state_->wakeup.wakeup();
}

template <typename T>
void Promise<T>::success(T&& value) {
  std::unique_lock<std::mutex> lk(state_->mutex);
  state_->value = new (state_->value_data) T(std::move(value));
  lk.unlock();
  state_->wakeup.wakeup();
}


/*
  void ;
  void success(T&& value);

*/

} // namespace fnord

#endif

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
Future<T>::Future(AutoRef<PromiseState<T>> promise_state) {}

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

/*
  bool Future<T>::isFailure() const;
  bool Future<T>::isSuccess() const;

  void onFailure(std::function<void> fn);
  void onSuccess(std::function<void> fn);

  void wait() const;
  void wait(const Duration& timeout) const;

  void onReady(std::function<void> fn);
  void onReady(TaskScheduler* scheduler, std::function<void> fn);

  const T& get() const;
*/


template <typename T>
Promise<T>::Promise() : state_(new PromiseState<T>()) {}

template <typename T>
Promise<T>::Promise(const Promise<T>& other) : state_(other.state_) {}

template <typename T>
Promise<T>::Promise(Promise<T>&& other) : state_(std::move(other.state_)) {}

template <typename T>
Promise<T>::~Promise() {}

/*
  void success(const T& value);
  void success(T&& value);
  void failure(const std::exception& e);

  Future<T> future() const;
*/

} // namespace fnord

#endif

/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _STX_BASE_OPTION_IMPL_H
#define _STX_BASE_OPTION_IMPL_H
namespace clip {

template <typename T>
Option<T>::Option() : value_(nullptr) {}

template <typename T>
Option<T>::Option(std::nullptr_t n) : value_(nullptr) {}

template <typename T>
Option<T>::Option(
    const T& value) :
    value_(new (value_data_) T(value)) {}

template <typename T>
Option<T>::Option(
    T&& value) :
    value_(new (value_data_) T(std::move(value))) {}

template <typename T>
Option<T>::Option(const Option<T>& other) {
  if (other.value_ == nullptr) {
    value_ = nullptr;
  } else {
    value_ = new (value_data_) T(*other.value_);
  }
}

template <typename T>
Option<T>::Option(Option<T>&& other) {
  if (other.value_ == nullptr) {
    value_ = nullptr;
  } else {
    value_ = new (value_data_) T(std::move(*other.value_));
    other.value_->~T();
    other.value_ = nullptr;
  }
}

template <typename T>
Option<T>::~Option() {
  if (value_ != nullptr) {
    value_->~T();
  }
}

template <typename T>
Option<T>& Option<T>::operator=(const Option<T>& other) {
  if (value_ != nullptr) {
    value_->~T();
  }

  if (other.value_ == nullptr) {
    value_ = nullptr;
  } else {
    value_ = new (value_data_) T(*other.value_);
  }

  return *this;
}

template <typename T>
Option<T>& Option<T>::operator=(Option<T>&& other) {
  if (value_ != nullptr) {
    value_->~T();
  }

  if (other.value_ == nullptr) {
    value_ = nullptr;
  } else {
    value_ = new (value_data_) T(std::move(*other.value_));
    other.value_->~T();
    other.value_ = nullptr;
  }

  return *this;
}

template <typename T>
Option<T>::operator bool() const {
  return value_;
}

template <typename T>
bool Option<T>::operator!() const {
  return !value_;
}

template <typename T>
T& Option<T>::operator*() {
  return *value_;
}

template <typename T>
const T& Option<T>::operator*() const {
  return *value_;
}

template <typename T>
T* Option<T>::operator->() {
  return value_;
}

template <typename T>
const T* Option<T>::operator->() const {
  return value_;
}

template <typename T>
T& Option<T>::get() const {
  if (value_ == nullptr) {
    RAISE(kRuntimeError, "get() called on empty option");
  }

  return *value_;
}

template <typename T>
const T& Option<T>::value_or(const T& alternative) const {
  if (value_) {
    return *value_;
  } else {
    return alternative;
  }
}

template <typename T>
bool Option<T>::isEmpty() const {
  return value_ == nullptr;
}

template <typename T>
Option<T> Some(const T& value) {
  return Option<T>(value);
}

template <typename T>
Option<T> None() {
  return Option<T>();
}

} // namespace clip
#endif

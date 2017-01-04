/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_BASE_OPTION_IMPL_H
#define _STX_BASE_OPTION_IMPL_H

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
T& Option<T>::get() const {
  if (value_ == nullptr) {
    RAISE(kRuntimeError, "get() called on empty option");
  }

  return *value_;
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

template <typename T>
std::string inspect(const Option<T> value) {
  if (value.isEmpty()) {
    return "<None>";
  } else {
    return StringUtil::format("<Some($0)>", inspect(value.get()));
  }
}

#endif

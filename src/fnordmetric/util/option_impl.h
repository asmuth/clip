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

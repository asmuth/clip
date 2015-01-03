/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_OPTION_IMPL_H
#define _FNORD_BASE_OPTION_IMPL_H

namespace fnord {

template <typename T>
Option<T>::Option() : value_(nullptr) {}

template <typename T>
Option<T>::Option(
    const T& value) :
    value_(new (value_data_) T(value)) {}

template <typename T>
Option<T>::Option(const Option<T>& other) {
  if (other.value_ == nullptr) {
    value_ = nullptr;
  } else {
    value_ = new (value_data_) T(*other.value_);
  }
}

template <typename T>
const T& Option<T>::get() const {
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

}
#endif

/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _STX_BASE_OPTION_IMPL_H
#define _STX_BASE_OPTION_IMPL_H
namespace fviz {

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

} // namespace fviz
#endif

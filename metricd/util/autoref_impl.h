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
#pragma once
#include "exception.h"

template <typename T>
AutoRef<T>::AutoRef() : ref_(nullptr) {}

template <typename T>
AutoRef<T>::AutoRef(std::nullptr_t) : ref_(nullptr) {}

template <typename T>
AutoRef<T>::AutoRef(T* ref) : ref_(ref) {
  if (ref_) ref_->incRef();
}

template <typename T>
AutoRef<T>::AutoRef(const AutoRef<T>& other) : ref_(other.ref_) {
  if (ref_) ref_->incRef();
}

template <typename T>
AutoRef<T>::AutoRef(AutoRef<T>&& other) : ref_(other.ref_) {
  other.ref_ = nullptr;
}

template <typename T>
AutoRef<T>& AutoRef<T>::operator=(const AutoRef<T>& other) {
  if (ref_ != nullptr) {
    ref_->decRef();
  }

  ref_ = other.ref_;
  if (ref_) {
    ref_->incRef();
  }

  return *this;
}

template <typename T>
AutoRef<T>::~AutoRef() {
  if (ref_ != nullptr) {
    ref_->decRef();
  }
}

template <typename T>
T& AutoRef<T>::operator*() const {
  return *ref_;
}

template <typename T>
T* AutoRef<T>::operator->() const {
  return ref_;
}

template <typename T>
void AutoRef<T>::reset(T* ref) {
  if (ref_ != nullptr) {
    ref_->decRef();
  }

  ref_ = ref;
  if (ref_) {
    ref_->incRef();
  }
}

template <typename T>
T* AutoRef<T>::get() const {
  return ref_;
}

template <typename T>
T* AutoRef<T>::release() {
  auto ref = ref_;
  ref_ = nullptr;
  return ref;
}

template <typename T>
bool AutoRef<T>::isNull() const {
  return ref_ == nullptr;
}

template <typename T>
template <typename T_>
AutoRef<T_> AutoRef<T>::asInstanceOf() const {
  auto cast = dynamic_cast<T_*>(ref_);
  if (cast == nullptr) {
    RAISE(kTypeError, "can't make referenced pointer into requested type");
  }

  return AutoRef<T_>(cast);
}

template <typename T>
template <typename T_>
bool AutoRef<T>::isInstanceOf() const {
  return dynamic_cast<T_*>(ref_) != nullptr;
}


template <typename T>
AutoRef<T> mkRef(T* ptr) {
  return AutoRef<T>(ptr);
}

template <typename T>
AutoRef<T> mkRef(ScopedPtr<T>&& ptr) {
  return AutoRef<T>(ptr.release());
}

template <typename T>
ScopedPtr<T> mkScoped(T* ptr) {
  return ScopedPtr<T>(ptr);
}


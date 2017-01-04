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


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_AUTOREF_IMPL_H
#define _FNORD_BASE_AUTOREF_IMPL_H

namespace fnord {

template <typename T>
AutoRef<T>::AutoRef(std::nullptr_t) : ref_(nullptr) {}

template <typename T>
AutoRef<T>::AutoRef(T* ref) : ref_(ref) {
  ref_->incRef();
}

template <typename T>
AutoRef<T>::AutoRef(const AutoRef<T>& other) : ref_(other.ref_) {
  ref_->incRef();
}

template <typename T>
AutoRef<T>::AutoRef(AutoRef<T>&& other) : ref_(other.ref_) {
  other.ref_ = nullptr;
}

template <typename T>
AutoRef<T>::~AutoRef() {
  if (ref_ != nullptr) {
    ref_->decRef();
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

} // namespace fnord
#endif

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
#ifndef _STX_BASE_AUTOREF_H
#define _STX_BASE_AUTOREF_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <atomic>
#include "stdtypes.h"

template <typename T>
class AutoRef {
public:
  using ValueType = T;

  AutoRef();
  AutoRef(std::nullptr_t);

  AutoRef(T* ref);

  template <
      class T1,
      typename std::enable_if<std::is_base_of<T, T1>::value, T>::type* = nullptr>
  AutoRef(ScopedPtr<T1>&& ref) : ref_(ref.release()) {
    if (ref_) ref_->incRef();
  }

  AutoRef(const AutoRef<T>& other);
  AutoRef(AutoRef<T>&& other);

  ~AutoRef();
  AutoRef<T>& operator=(const AutoRef<T>& other);

  T& operator*() const;
  T* operator->() const;

  void reset(T*);

  T* get() const;
  T* release();
  bool isNull() const;

  template <typename T_>
  AutoRef<T_> asInstanceOf() const;

  template <typename T_>
  bool isInstanceOf() const;

protected:
  T* ref_;
};

class RefCounted {
public:
  RefCounted();
  virtual ~RefCounted() {}

  void incRef();
  bool decRef();
protected:
  mutable std::atomic<unsigned> refcount_;
};

template <typename T>
using RefPtr = AutoRef<T>;

template <typename T>
using RefPtrVector = Vector<AutoRef<T>>;

using AnyRef = RefPtr<RefCounted>;

template <typename T>
AutoRef<T> mkRef(T* ptr);

template <typename T>
AutoRef<T> mkRef(ScopedPtr<T>&& ptr);

template <typename T>
ScopedPtr<T> mkScoped(T* ptr);

#include "autoref_impl.h"
#endif

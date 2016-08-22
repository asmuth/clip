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
#ifndef _STX_BASE_AUTOREF_H
#define _STX_BASE_AUTOREF_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <atomic>
#include <fnordmetric/util/stdtypes.h>

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

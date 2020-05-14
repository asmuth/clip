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
#ifndef _STX_BASE_OPTION_H
#define _STX_BASE_OPTION_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include "exception.h"

namespace clip {

template <typename T>
class Option {
public:
  Option();
  Option(std::nullptr_t n);
  Option(const T& value);
  Option(T&& value);
  Option(const Option<T>& other);
  Option(Option<T>&& other);
  ~Option();

  Option& operator=(const Option<T>& other);
  Option& operator=(Option<T>&& other);

  operator bool() const;
  bool operator!() const;
  T& operator*();
  const T& operator*() const;
  T* operator->();
  const T* operator->() const;

  bool isEmpty() const;
  T& get() const;

protected:
  char value_data_[sizeof(T)];
  T* value_;
};

template <typename T>
Option<T> Some(const T& value);

template <typename T>
Option<T> None();

} // namespace clip

#include "option_impl.h"
#endif

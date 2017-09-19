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
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include "inspect.h"
#include "exception.h"

template <typename T>
class Option {
public:
  using ValueType = typename std::remove_reference<T>::type;

  Option();
  Option(std::nullptr_t n);
  Option(T value);
  Option(const Option<T>& other);
  Option(Option<T>&& other);
  ~Option();

  Option& operator=(const Option<T>& other);
  Option& operator=(Option<T>&& other);
  operator bool() const;

  bool isEmpty() const;
  bool isNone() const;
  bool isSome() const;
  T& get() const;

protected:
  char value_data_[sizeof(T)];
  ValueType* value_;
};

template <typename T>
Option<T> Some(const T& value);

template <typename T>
Option<T> Some(T&& value);

template <typename T>
Option<T> None();

template <typename MapType>
Option<typename MapType::key_type> lookup(
    const MapType& map,
    const typename MapType::key_type& key);

template <typename MapType>
typename MapType::mapped_type lookup(
    const MapType& map,
    const typename MapType::key_type& key,
    const typename MapType::mapped_type& fallback);

#include "option_impl.h"


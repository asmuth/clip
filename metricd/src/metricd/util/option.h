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
#ifndef _STX_BASE_OPTION_H
#define _STX_BASE_OPTION_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include "metricd/util/inspect.h"
#include "metricd/util/exception.h"

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

#include "option_impl.h"
#endif

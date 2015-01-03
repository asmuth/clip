/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_OPTION_H
#define _FNORD_BASE_OPTION_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include "fnord/base/inspect.h"
#include "fnord/base/exception.h"

namespace fnord {

template <typename T>
class Option {
public:
  Option();
  Option(const T& value);

  Option(const Option<T>& other);
  Option(const Option<T>&& other);
  Option& operator=(const Option<T>& other) = delete;

  bool isEmpty() const;
  const T& get() const;

protected:
  char value_data_[sizeof(T)];
  T* value_;
};

template <typename T>
Option<T> Some(const T& value);

template <typename T>
Option<T> None();

} // namespace fnord

#include "option_impl.h"
#endif

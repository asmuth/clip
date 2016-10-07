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

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V. <laura@eventql.io>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <iostream>
#include "metricd/util/return_code.h"
#include "metricd/util/stringutil.h"

namespace fnordmetric {

template <typename T>
const T* DataFrame::getData() const {
  return (const T*) data_;
}

template <typename T>
T* DataFrame::getData() {
  return (T*) data_;
}

template <typename T>
void DataFrame::addValue(T value, uint64_t time /* = 0 */) {
  auto pos = size_;
  resize(size_ + 1);
  ((T*) data_)[pos] = value;
  if (has_time_) {
    getTime()[pos] = time;
  }
}

} // namespace fnordmetric


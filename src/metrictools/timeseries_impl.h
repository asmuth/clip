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
#include <assert.h>
#include <iostream>

namespace fnordmetric {

template <typename T>
size_t Timeseries<T>::size() const {
  assert(values.size() == timestamps.size());
  return timestamps.size();
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<std::is_same<T1, T2>::value>::type* = nullptr>
ReturnCode convertTimeseries(
    const Timeseries<T1>& in,
    Timeseries<T2>* out) {
  *out = in;
  return ReturnCode::success();
}

template <typename T>
ReturnCode convertTimeseries(
    const AnyTimeseries* in,
    Timeseries<T>* out) {
  if (dynamic_cast<const Timeseries<std::string>*>(in)) {
    return convertTimeseries(*dynamic_cast<const Timeseries<std::string>*>(in), out);
  }

  if (dynamic_cast<const Timeseries<uint64_t>*>(in)) {
    return convertTimeseries(*dynamic_cast<const Timeseries<uint64_t>*>(in), out);
  }

  if (dynamic_cast<const Timeseries<int64_t>*>(in)) {
    return convertTimeseries(*dynamic_cast<const Timeseries<int64_t>*>(in), out);
  }

  if (dynamic_cast<const Timeseries<double>*>(in)) {
    return convertTimeseries(*dynamic_cast<const Timeseries<double>*>(in), out);
  }

  return ReturnCode::error("EARG", "invalid timeseries type");
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const Timeseries<T>& op) {
  return out;
}

} // namespace fnordmetric


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

template <typename T>
void Timeseries<T>::clear() {
  values.clear();
  timestamps.clear();
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

  assert(false);
  return ReturnCode::error("EARG", "invalid timeseries type");
}

template <typename T1, typename T2>
ReturnCode convertTimeseriesFromTo(TimeseriesRef* ts, Timeseries<T2>** out) {
  if (!dynamic_cast<const Timeseries<T1>*>(ts->get())) {
    return ReturnCode::success();
  }

  if (std::is_same<T1, T2>::value) {
    *out = dynamic_cast<Timeseries<T2>*>(ts->get());
    assert(*out);
    return ReturnCode::success();
  }

  *out = new Timeseries<T2>();
  std::unique_ptr<Timeseries<T2>> out_ref(*out);

  auto rc = convertTimeseries(
      dynamic_cast<const Timeseries<T1>*>(ts->get()),
      out_ref.get());

  if (!rc.isSuccess()) {
    return rc;
  }

  *ts = std::move(out_ref);
  return ReturnCode::success();
}

template <typename T>
ReturnCode convertTimeseriesTo(TimeseriesRef* ts, Timeseries<T>** out) {
  *out = nullptr;

  assert(ts->get());

  {
    auto rc = convertTimeseriesFromTo<std::string, T>(ts, out);
    if (!rc.isSuccess() || *out) {
      return rc;
    }
  }

  {
    auto rc = convertTimeseriesFromTo<double, T>(ts, out);
    if (!rc.isSuccess() || *out) {
      return rc;
    }
  }

  {
    auto rc = convertTimeseriesFromTo<int64_t, T>(ts, out);
    if (!rc.isSuccess() || *out) {
      return rc;
    }
  }

  {
    auto rc = convertTimeseriesFromTo<uint64_t, T>(ts, out);
    if (!rc.isSuccess() || *out) {
      return rc;
    }
  }

  assert(false);
  return ReturnCode::error("EARG", "invalid timeseries type");
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const Timeseries<T>& op) {
  return out;
}

} // namespace fnordmetric


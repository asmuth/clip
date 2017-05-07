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
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <metrictools/util/return_code.h>

namespace fnordmetric {

struct AnyTimeseries {
  virtual ~AnyTimeseries() = default;
};

using TimeseriesRef = std::unique_ptr<AnyTimeseries>;

template <typename T>
struct Timeseries : public AnyTimeseries {

  /**
   * A list of timestamps, formatted as the number of microseconds since epoch
   */
  std::vector<uint64_t> timestamps;
  
  /**
   * A list of values, one for each element in the timestamps vector
   */
  std::vector<T> values;

  /**
   * Return the number of elements
   */
  size_t size() const;

  /**
   * Clear all elements
   */
  void clear();

};

/**
 * Convert a timeseries from one value type to another
 */
template <typename T>
ReturnCode convertTimeseries(
    const AnyTimeseries* in,
    Timeseries<T>* out);

/**
 * Convert a timeseries from one value type to another
 */
template <
    typename T1,
    typename T2,
    typename std::enable_if<!std::is_same<T1, T2>::value>::type* = nullptr>
ReturnCode convertTimeseries(
    const Timeseries<T1>& in,
    Timeseries<T2>* out);

/**
 * Convert a timeseries to another type (in place)
 */
template <typename T>
ReturnCode convertTimeseriesTo(TimeseriesRef* ts, Timeseries<T>** out);

/**
 * Calculate a rate
 */
void timeseriesRate(
    Timeseries<double>* timeseries,
    uint64_t rate);

/**
 * Print a debug dump of the timeseries
 */
template <typename T>
std::ostream& operator<<(std::ostream& out, const Timeseries<T>& op);

} // namespace fnordmetric

#include "timeseries_impl.h"


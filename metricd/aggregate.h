/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metricd/sample.h>

namespace fnordmetric {

class OutputAggregator {
public:

  virtual ~OutputAggregator() = default;

  virtual bool aggregateUINT64(
      uint64_t input_time,
      uint64_t input_value,
      uint64_t* output_time,
      uint64_t* output_value) = 0;

};

template <typename T>
class SumOutputAggregator : public OutputAggregator {
public:

  SumOutputAggregator();

  bool aggregateUINT64(
      uint64_t input_time,
      uint64_t input_value,
      uint64_t* output_time,
      uint64_t* output_value) override;

  bool aggregate(
      uint64_t input_time,
      T input_value,
      uint64_t* output_time,
      T* output_value);

protected:
  uint64_t twin_;
  T sum_;
};

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align);

} // namespace fnordmetric

#include "aggregate_impl.h"


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/aggregate.h"
#include <limits>

namespace fnordmetric {

template <>
bool SumOutputAggregator<uint64_t>::aggregateUINT64(
    uint64_t input_time,
    uint64_t input_value,
    uint64_t* output_time,
    uint64_t* output_value) {
  return aggregate(input_time, input_value, output_time, output_value);
}

SumInputAggregator::SumInputAggregator(
    uint64_t granularity,
    uint64_t align /* = 0 */) :
    granularity_(granularity),
    align_(align) {}

ReturnCode SumInputAggregator::addSample(
    tsdb::Cursor* cursor,
    uint64_t time,
    MetricDataType value_type,
    const void* value,
    size_t value_len) {

}

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align = 0) {
  if (window >= std::numeric_limits<uint64_t>::max() / 2 ||
      timestamp >= std::numeric_limits<uint64_t>::max() / 2) {
    return -1;
  }

  int64_t timestamp_base = int64_t(timestamp) - int64_t(align);
  return (timestamp_base / int64_t(window)) * int64_t(window) + int64_t(align);
}

} // namespace fnordmetric


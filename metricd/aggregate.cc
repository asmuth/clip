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

namespace fnordmetric {

template <>
bool SumOutputAggregator<uint64_t>::aggregateUINT64(
    uint64_t input_time,
    uint64_t input_value,
    uint64_t* output_time,
    uint64_t* output_value) {
  return aggregate(input_time, input_value, output_time, output_value);
}

} // namespace fnordmetric


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdexcept>
#include <assert.h>
#include "metricd/types.h"

namespace fnordmetric {

void tval_add(
    MetricDataType type,
    void* reg,
    size_t reg_len,
    const void* op,
    size_t op_len) {
  switch (type) {

    case MetricDataType::UINT64: {
      assert(reg_len == sizeof(uint64_t));
      assert(op_len == sizeof(uint64_t));
      *((uint64_t*) reg) += *((const uint64_t*) op);
      return;
    }

    default: throw std::invalid_argument("type error");
  }
}

size_t getMetricDataTypeSize(MetricDataType t) {
  switch (t) {
    case MetricDataType::UINT64: return sizeof(uint64_t);
    default: throw std::invalid_argument("type error");
  }
}

} // namespace fnordmetric


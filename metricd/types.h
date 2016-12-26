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
#pragma once
#include <stdlib.h>
#include <stdint.h>

namespace fnordmetric {

enum class MetricDataType {
  UINT64,
  INT64,
  FLOAT64
};

void tval_zero(MetricDataType type, void* reg, size_t reg_len);

void tval_add(
    MetricDataType type,
    void* reg,
    size_t reg_len,
    const void* op,
    size_t op_len);

int tval_cmp(
    MetricDataType type,
    void* left,
    size_t left_len,
    const void* right,
    size_t right_len);

size_t getMetricDataTypeSize(MetricDataType t);

} // namespace fnordmetric


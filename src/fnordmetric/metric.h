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
#include <fnordmetric/sample.h>
#include <fnordmetric/util/return_code.h>
#include <fnordmetric/util/option.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <mutex>

namespace fnordmetric {
class MetricMap;

using MetricIDType = std::string;
using SensorIDType = std::vector<std::string>;

enum MetricKind : uint32_t {
  SAMPLE_UINT64     = 1,
  SAMPLE_INT64      = 2,
  SAMPLE_FLOAT64    = 3,
  SAMPLE_STRING     = 4,
  MONOTONIC_UINT64  = 5,
  MONOTONIC_INT64   = 6,
  MONOTONIC_FLOAT64 = 7
};

std::string getMetricKindName(MetricKind t);
bool parseMetricKind(const std::string& s, MetricKind* t);

struct MetricConfig {
  std::string metric_id;
  MetricKind kind;
  std::vector<std::string> sensor_id_labels;
  std::string unit_id;
};

} // namespace fnordmetric


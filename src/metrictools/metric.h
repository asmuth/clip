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
#include <metrictools/util/return_code.h>
#include <metrictools/util/option.h>
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

enum class MetricKind {
  GAUGE_UINT64,
  GAUGE_INT64,
  GAUGE_FLOAT64,
  MONOTONIC_UINT64,
  MONOTONIC_FLOAT64,
  COUNTER_UINT64,
  COUNTER_INT64,
  COUNTER_FLOAT64,
  STRING
};

struct MetricLabelConfig {
  std::vector<std::string> labels;
};

struct MetricLabels {
  std::vector<std::string> labels;
  std::vector<std::string> values;
};

struct MetricLabelOverride {
  std::string label;
  std::string value;
  bool is_default;
};

using MetricLabelOverrideList = std::vector<MetricLabelOverride>;

struct MetricConfig {
  MetricConfig();
  std::string metric_id;
  MetricKind kind;
  MetricLabelConfig label_config;
  std::vector<MetricLabelOverride> label_overrides;
  std::string unit_id;
  uint64_t granularity;
  uint64_t rate;
};

std::string getMetricKindName(MetricKind t);
bool parseMetricKind(const std::string& s, MetricKind* t);

std::ostream& operator<<(std::ostream& out, const MetricLabels& path);

} // namespace fnordmetric


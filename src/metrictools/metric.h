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
#include <metrictools/sample.h>
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

enum class MetricDataType {
  UINT64, INT64, FLOAT64, STRING
};

enum class MetricReportingScheme {
 SAMPLE, MONOTONIC
};

struct MetricKind {
  MetricDataType type;
  MetricReportingScheme scheme;
};

struct MetricLabelConfig {
  std::vector<std::string> labels;
};

struct MetricLabels {
  std::vector<std::string> labels;
  std::vector<std::string> values;
};

struct MetricConfig {
  MetricConfig();
  std::string metric_id;
  MetricKind kind;
  MetricLabelConfig label_config;
  std::string unit_id;
};

std::string getMetricDataTypeName(MetricDataType t);
bool parseMetricDataType(const std::string& s, MetricDataType* t);

std::string getMetricReportingSchemeName(MetricReportingScheme t);
bool parseMetricReportingScheme(const std::string& s, MetricReportingScheme* t);

std::ostream& operator<<(std::ostream& out, const MetricLabels& path);

} // namespace fnordmetric


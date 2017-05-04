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
#include <assert.h>
#include <sstream>
#include <metrictools/metric.h>
#include <metrictools/metric_map.h>
#include <metrictools/util/logging.h>

namespace fnordmetric {

MetricConfig::MetricConfig() :
    kind{MetricDataType::FLOAT64, MetricReportingScheme::SAMPLE} {}

std::string getMetricDataTypeName(MetricDataType t) {
  switch (t) {
    case MetricDataType::UINT64: return "uint64";
    case MetricDataType::INT64: return "int64";
    case MetricDataType::FLOAT64: return "float64";
    case MetricDataType::STRING: return "string";
  }

  return "???";
}

bool parseMetricDataType(
    const std::string& s,
    MetricDataType* t) {
  if (s == "uint64") { *t = MetricDataType::UINT64; return true; }
  if (s == "int64") { *t = MetricDataType::INT64; return true; }
  if (s == "float64") { *t = MetricDataType::FLOAT64; return true; }
  if (s == "string") { *t = MetricDataType::STRING; return true; }
  return false;
}

std::string getMetricReportingSchemeName(MetricReportingScheme t) {
  switch (t) {
    case MetricReportingScheme::SAMPLE: return "sample";
    case MetricReportingScheme::MONOTONIC: return "monotonic";
  }

  return "???";
}

bool parseMetricReportingScheme(
    const std::string& s,
    MetricReportingScheme* t) {
  if (s == "sample") { *t = MetricReportingScheme::SAMPLE; return true; }
  if (s == "monotonic") { *t = MetricReportingScheme::MONOTONIC; return true; }
  return false;
}

std::ostream& operator<<(std::ostream& out, const MetricLabels& path) {
  assert(path.labels.size() == path.values.size());
  out << "[";
  for (size_t i = 0; i < path.labels.size(); ++i) {
    if (i > 0) {
      out << ", ";
    }

    out << "<" << path.labels[i] << ": '" << path.values[i] << "'>";
  }

  out << "]";
  return out;
}

} // namespace fnordmetric


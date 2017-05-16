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
    kind{MetricKind::GAUGE_FLOAT64},
    granularity(0),
    rate(0) {}

std::string getMetricKindName(MetricKind t) {
  static const std::map<MetricKind, std::string> kind_map = {
    { MetricKind::GAUGE_UINT64,            "gauge(uint64)" },
    { MetricKind::GAUGE_INT64,             "gauge(int64)" },
    { MetricKind::GAUGE_FLOAT64,           "gauge(float64)" },
    { MetricKind::MONOTONIC_UINT64,        "monotonic(uint64)" },
    { MetricKind::MONOTONIC_FLOAT64,       "monotonic(float64)" },
    { MetricKind::COUNTER_UINT64,          "counter(uint64)" },
    { MetricKind::COUNTER_INT64,           "counter(int64)" },
    { MetricKind::COUNTER_FLOAT64,         "counter(float64)" },
    { MetricKind::STRING,                  "string" }
  };

  auto iter = kind_map.find(t);
  if (iter != kind_map.end()) {
    return iter->second;
  } else {
    return "???";
  }
}

bool parseMetricKind(const std::string& s, MetricKind* t) {
  static const std::map<std::string, MetricKind> kind_map = {
    { "gauge(uint64)",      MetricKind::GAUGE_UINT64 },
    { "gauge(int64)",       MetricKind::GAUGE_INT64 },
    { "gauge(float64)",     MetricKind::GAUGE_FLOAT64 },
    { "monotonic(uint64)",  MetricKind::MONOTONIC_UINT64 },
    { "monotonic(float64)", MetricKind::MONOTONIC_FLOAT64 },
    { "counter(uint64)",    MetricKind::COUNTER_UINT64 },
    { "counter(int64)",     MetricKind::COUNTER_INT64 },
    { "counter(float64)",   MetricKind::COUNTER_FLOAT64 },
    { "string",             MetricKind::STRING }
  };

  auto iter = kind_map.find(s);
  if (iter != kind_map.end()) {
    *t = iter->second;
    return true;
  } else {
    return false;
  }
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


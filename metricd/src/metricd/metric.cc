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
#include <metricd/metric.h>
#include <metricd/metric_map.h>
#include <metricd/metric_cursor.h>
#include <metricd/util/logging.h>
#include <libtsdb/varint.h>

namespace fnordmetric {

MetricConfig::MetricConfig() :
    kind(MetricKind::UNKNOWN),
    granularity(0),
    display_granularity(0),
    is_valid(false) {}

Metric::Metric(
    const std::string& key) :
    key_(key),
    unit_config_(nullptr) {}

ReturnCode Metric::setConfig(std::shared_ptr<const MetricConfig> config) {
  std::unique_lock<std::mutex> lk(config_mutex_);
  //if (config->aggregation != MetricAggregationType::NONE &&
  //    config->granularity == 0) {
  //  logWarning(
  //      "metric<$0>: setting 'aggregation' without 'granularity' will have "
  //      "no effect",
  //      key_);
  //}

  if (config->kind == MetricKind::UNKNOWN) {
    return ReturnCode::errorf(
        "EARG",
        "metric<$0>: missing 'kind'",
        key_);
  }

  config_ = config;
  return ReturnCode::success();
}

std::shared_ptr<const MetricConfig> Metric::getConfig() const {
  std::unique_lock<std::mutex> lk(config_mutex_);
  return config_;
}

void Metric::setUnitConfig(std::shared_ptr<const UnitConfig> config) {
  std::unique_lock<std::mutex> lk(config_mutex_);
  unit_config_ = config;
}

std::shared_ptr<const UnitConfig> Metric::getUnitConfig() const {
  std::unique_lock<std::mutex> lk(config_mutex_);
  return unit_config_;
}

MetricInfo::MetricInfo() : metric_(nullptr) {}

MetricInfo::MetricInfo(
    Metric* metric,
    std::shared_ptr<MetricMap> metric_map) :
    metric_(metric),
    metric_map_(std::move(metric_map)) {}

MetricInfo::MetricInfo(
    MetricInfo&& o) :
    metric_(o.metric_),
    metric_map_(std::move(o.metric_map_)) {
  o.metric_ = nullptr;
}

MetricInfo& MetricInfo::operator=(MetricInfo&& o) {
  metric_map_ = std::move(o.metric_map_);
  metric_ = o.metric_;
  o.metric_ = nullptr;
  return *this;
}

std::shared_ptr<const UnitConfig> MetricInfo::getUnitConfig() const {
  if (!metric_) {
    return nullptr;
  }

  return metric_->getUnitConfig();
}

std::shared_ptr<const MetricConfig> MetricInfo::getMetricConfig() const {
  if (!metric_) {
    return nullptr;
  }

  return metric_->getConfig();
}

tval_type getMetricDataType(MetricKind t) {
  switch (t) {
    case SAMPLE_UINT64: return tval_type::UINT64;
    case SAMPLE_INT64: return tval_type::INT64;
    case SAMPLE_FLOAT64: return tval_type::FLOAT64;
    case COUNTER_UINT64: return tval_type::UINT64;
    case COUNTER_INT64: return tval_type::INT64;
    case COUNTER_FLOAT64: return tval_type::FLOAT64;
    case MONOTONIC_UINT64: return tval_type::UINT64;
    case MONOTONIC_INT64: return tval_type::INT64;
    case MONOTONIC_FLOAT64: return tval_type::FLOAT64;
    case MIN_UINT64: return tval_type::UINT64;
    case MIN_INT64: return tval_type::INT64;
    case MIN_FLOAT64: return tval_type::FLOAT64;
    case MAX_UINT64: return tval_type::UINT64;
    case MAX_INT64: return tval_type::INT64;
    case MAX_FLOAT64: return tval_type::FLOAT64;
    case AVERAGE_UINT64: return tval_type::UINT64;
    case AVERAGE_INT64: return tval_type::INT64;
    case AVERAGE_FLOAT64: return tval_type::FLOAT64;

    case UNKNOWN:
    default:
      return tval_type::UINT64;
  }
}

GroupSummaryMethod getMetricDefaultGroupSumamry(MetricKind t) {
  switch (t) {
    case SAMPLE_UINT64:
    case SAMPLE_INT64:
    case SAMPLE_FLOAT64:
    case COUNTER_UINT64:
    case COUNTER_INT64:
    case COUNTER_FLOAT64:
    case MONOTONIC_UINT64:
    case MONOTONIC_INT64:
    case MONOTONIC_FLOAT64:
    case MIN_UINT64:
    case MIN_INT64:
    case MIN_FLOAT64:
    case MAX_UINT64:
    case MAX_INT64:
    case MAX_FLOAT64:
    case AVERAGE_UINT64:
    case AVERAGE_INT64:
    case AVERAGE_FLOAT64:
    case UNKNOWN:
    default:
      return GroupSummaryMethod::SUM;
  }
}

} // namespace fnordmetric


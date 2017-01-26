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
#include <atomic>
#include <memory>
#include <string>
#include "metricd/metric.h"
#include "metricd/metric_cursor.h"
#include "metricd/metric_map.h"
#include "metricd/config_list.h"
#include "metricd/util/return_code.h"
#include "libtsdb/tsdb.h"

namespace fnordmetric {

class MetricService {
public:

  static ReturnCode startService(
      const std::string& datadir,
      std::unique_ptr<MetricService>* service);

  ReturnCode applyConfig(const ConfigList* config);

  /**
   * List all metrics
   */
  MetricListCursor listMetrics() const;

  /**
   * Create/configure metric
   */
  ReturnCode configureMetric(
      const std::string& metric_id,
      std::shared_ptr<MetricConfig> config);

  /**
   * Remove metric
   */
  ReturnCode configureMetric(const std::string& metric_id);

  /**
   * Get information about a specific metric
   */
  ReturnCode describeMetric(
      const std::string& metric_id,
      MetricInfo* metric_info);

  /**
   * Get a cursor to a metric
   */
  ReturnCode fetchData(
      const MetricIDType& metric_id,
      const MetricCursorOptions& opts,
      MetricCursor* cursor);

  /**
   * Insert a sample into a metric series
   */
  ReturnCode insertSample(
      const MetricIDType& metric_id,
      uint64_t time,
      const std::string& value);

  struct BatchInsertOptions {
    BatchInsertOptions();
    bool metric_id_rewrite_enabled;
    std::regex metric_id_rewrite_regex;
    std::string metric_id_rewrite_replace;
    bool series_id_rewrite_enabled;
    std::regex series_id_rewrite_regex;
    std::string series_id_rewrite_replace;
  };

  ReturnCode insertSamplesBatch(
      const char* data,
      size_t len,
      const BatchInsertOptions* opts = nullptr);

protected:

  MetricService(const std::string& datadir);
  std::string datadir_;
  VersionedMetricMap metric_map_;
  std::mutex metric_map_mutex_;
};

} // namespace fnordmetric


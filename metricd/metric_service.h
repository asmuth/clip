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
#include "metricd/config/config_list.h"
#include "metricd/util/return_code.h"
#include "libtsdb/tsdb.h"

namespace fnordmetric {

class MetricService {
public:

  static ReturnCode startService(
      const std::string& datadir,
      const ConfigList* config,
      std::unique_ptr<MetricService>* service);

  /**
   * List all metrics
   */
  MetricListCursor listMetrics();

  /**
   * List all series in a metric
   */
  ReturnCode listSeries(
      const MetricIDType& metric_id,
      MetricSeriesListCursor* cursor);

  /**
   * Insert a sample into a metric series
   */
  ReturnCode insertSample(
      const MetricIDType& metric_id,
      const SeriesNameType& series_name,
      uint64_t time,
      const std::string& value);

  /**
   * Get a cursor to a metric series
   */
  MetricSeriesCursor getCursor(
      const MetricIDType& metric_id,
      const SeriesIDType& series_id,
      uint64_t time_begin,
      uint64_t time_limit);

  /**
   * Get a cursor to a metric series
   */
  MetricSeriesCursor getCursor(
      const MetricIDType& metric_id,
      const SeriesNameType& series_name,
      uint64_t time_begin,
      uint64_t time_limit);

protected:

  MetricService(
      std::unique_ptr<tsdb::TSDB> tsdb,
      std::shared_ptr<MetricMap> metric_map,
      SeriesIDType series_id);

  std::unique_ptr<tsdb::TSDB> tsdb_;
  VersionedMetricMap metric_map_;
  SeriesIDProvider id_provider_;
  //tsdb::TSDB tsdb_;
};

} // namespace fnordmetric


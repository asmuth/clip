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
#include "metricd/metric_service.h"

namespace fnordmetric {

MetricService::MetricService(
    std::shared_ptr<MetricMap> metric_map) {
  metric_map_.updateMetricMap(std::move(metric_map));
}

ReturnCode MetricService::insertSample(
    const MetricIDType& metric_id,
    const LabelledSample& sample) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ERUNTIME", "metric not found");
  }

  std::shared_ptr<MetricSeries> series;
  auto rc = metric->getSeriesList()->findOrCreateSeries(
      &id_provider_,
      sample.getLabels(),
      &series);

  if (!rc.isSuccess()) {
    return rc;
  }

  return series->insertSample(sample.getSample());
}

//void MetricService::scanSamples(
//    const std::string& metric_key,
//    const fnord::DateTime& time_begin,
//    const fnord::DateTime& time_end,
//    std::function<void (const metricd::MetricSample& sample)> callback) {
//  auto partitions = tsdb::TimeWindowPartitioner::partitionKeysFor(
//      metric_key,
//      time_begin,
//      time_end,
//      600 * kMicrosPerSecond);
//
//  for (const auto& partition_key : partitions) {
//    tsdb_->fetchPartition(
//        tsdb_namespace_,
//        metric_key,
//        partition_key,
//        [callback] (const Buffer& buf) {
//      callback(msg::decode<metricd::MetricSample>(buf));
//    });
//  }
//}
//

} // namsepace fnordmetric


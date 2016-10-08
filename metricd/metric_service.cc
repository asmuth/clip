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
#include "metricd/util/fileutil.h"
#include "metricd/util/logging.h"

namespace fnordmetric {

ReturnCode MetricService::startService(
    const std::string& datadir,
    std::unique_ptr<MetricService>* service) {
  auto db_path = FileUtil::joinPaths(datadir, "default.tsdb");

  if (!FileUtil::exists(datadir)) {
    return ReturnCode::errorf("EIO", "datadir doesn't exist: $0", datadir);
  }

  /* open tsdb */
  std::unique_ptr<tsdb::TSDB> tsdb;
  if (FileUtil::exists(db_path)) {
    logInfo("Opening database: $0", db_path);
    if (!tsdb::TSDB::openDatabase(&tsdb, db_path)) {
      return ReturnCode::errorf("EIO", "can't open database at $0", db_path);
    }
  } else {
    logInfo("Creating database: $0", db_path);
    if (!tsdb::TSDB::createDatabase(&tsdb, db_path)) {
      return ReturnCode::errorf("EIO", "can't create database at $0", db_path);
    }
  }

  /* load metrics */
  std::set<uint64_t> series_ids;
  if (!tsdb->listSeries(&series_ids)) {
    return ReturnCode::error("ERUNTIME", "error while opening database");
  }

  SeriesIDType series_id_max = 0;
  for (auto s : series_ids) {
    if (s > series_id_max) {
      series_id_max = s;
    }
  }

  MetricMapBuilder metric_map_builder;
  for (const auto& series_id : series_ids) {
    std::string metadata_buf;
    if (!tsdb->getSeriesMetadata(series_id, &metadata_buf)) {
      return ReturnCode::error("ERUNTIME", "error while opening database");
    }

    std::istringstream metadata_is(metadata_buf);
    MetricSeriesMetadata metadata;
    if (!metadata.decode(&metadata_is)) {
      return ReturnCode::error("ERUNTIME", "corrupt database");
    }

    logDebug(
        "Opening timeseries; metric_id=$0; series_id=$1",
        metadata.metric_id,
        series_id);
  }

  /* initialize service */
  service->reset(
      new MetricService(
          std::move(tsdb),
          metric_map_builder.getMetricMap(),
          series_id_max));

  return ReturnCode::success();
}

MetricService::MetricService(
    std::unique_ptr<tsdb::TSDB> tsdb,
    std::shared_ptr<MetricMap> metric_map,
    SeriesIDType series_id) :
    tsdb_(std::move(tsdb)),
    id_provider_(series_id) {
  metric_map_.updateMetricMap(std::move(metric_map));
}

void MetricService::configureMetric(
    const MetricIDType& metric_id,
    const MetricConfig& config) {
  std::unique_lock<std::mutex> lk(metric_map_mutex_);

  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (metric) {
    metric->setConfig(config);
    return;
  }

  MetricMapBuilder metric_map_builder;
  metric_map_builder.copyFrom(metric_map.get());
  metric = new Metric(metric_id);
  metric->setConfig(config);
  metric_map_builder.addMetric(metric_id, std::unique_ptr<Metric>(metric));
  metric_map_.updateMetricMap(metric_map_builder.getMetricMap());
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
      tsdb_.get(),
      &id_provider_,
      metric_id,
      sample.getLabels(),
      &series);

  if (!rc.isSuccess()) {
    return rc;
  }

  rc = series->insertSample(tsdb_.get(), sample.getSample());
  if (rc.isSuccess()) {
    tsdb_->commit(); // FIXME
  }

  return rc;
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


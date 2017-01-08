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
#include "metricd/metric_service.h"
#include "metricd/util/fileutil.h"
#include "metricd/util/logging.h"

namespace fnordmetric {

ReturnCode MetricService::startService(
    const std::string& datadir,
    ConfigList* config,
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

  /* load metric configs */
  MetricMapBuilder metric_map_builder(nullptr);
  for (auto& mc : config->getMetricConfigs()) {
    auto metric = metric_map_builder.findMetric(mc.first);
    if (metric) {
      return ReturnCode::error(
          "EARG",
          StringUtil::format("duplicate metric for: $0", mc.first));
    }

    logDebug("Opening metric; metric_id=$0", mc.first);

    if (mc.second.summarize_group.isEmpty()) {
      mc.second.summarize_group = Some(
          getMetricDefaultGroupSumamry(mc.second.kind));
    }

    metric = new Metric(mc.first);
    auto rc = metric->setConfig(&mc.second);
    if (!rc.isSuccess()) {
      return rc;
    }

    auto unit = config->getUnitConfig(mc.second.unit_id);
    if (unit) {
      metric->setUnitConfig(unit);
    }

    metric_map_builder.addMetric(mc.first, std::unique_ptr<Metric>(metric));
  }

  /* load series */
  std::set<uint64_t> series_ids;
  if (!tsdb->listSeries(&series_ids)) {
    return ReturnCode::error("ERUNTIME", "error while opening database");
  }

  uint64_t series_id_max = 0;
  for (auto s : series_ids) {
    if (s > series_id_max) {
      series_id_max = s;
    }
  }

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

    const auto& metric_id = metadata.metric_id;
    auto metric = metric_map_builder.findMetric(metric_id);
    if (!metric) {
      logWarning(
          "Skipping orphaned series; metric_id=$0; series_id=$1",
          metric_id,
          series_id);

      continue;
    }

    if (metric->getConfig()->kind != metadata.metric_kind) {
      logWarning(
          "Skipping series with invalid type; metric_id=$0; series_id=$1",
          metric_id,
          series_id);

      continue;
    }

    logDebug(
        "Opening series; metric_id=$0; series_id=$1",
        metadata.metric_id,
        series_id);

    metric->getSeriesList()->addSeries(
        SeriesIDType(series_id),
        metadata.series_name);
  }

  /* initialize service */
  service->reset(
      new MetricService(
          std::move(tsdb),
          metric_map_builder.getMetricMap(),
          SeriesIDType(series_id_max)));

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

MetricListCursor MetricService::listMetrics() {
  auto metric_map = metric_map_.getMetricMap();
  return MetricListCursor(metric_map);
}

ReturnCode MetricService::describeMetric(
    const std::string& metric_id,
    MetricInfo* metric_info) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ENOTFOUND", "metric not found");
  }

  *metric_info = MetricInfo(metric, metric_map);
  return ReturnCode::success();
}

ReturnCode MetricService::listSeries(
    const MetricIDType& metric_id,
    MetricSeriesListCursor* cursor) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ENOTFOUND", "metric not found");
  }

  auto series_list = metric->getSeriesList();

  std::vector<SeriesIDType> snapshot;
  series_list->listSeries(&snapshot);

  *cursor = MetricSeriesListCursor(
      series_list,
      std::move(snapshot));

  cursor->setMetricMap(std::move(metric_map));

  return ReturnCode::success();
}

ReturnCode MetricService::fetchData(
    const MetricIDType& metric_id,
    const MetricCursorOptions& opts,
    MetricCursor* cursor) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ENOTFOUND", "metric not found");
  }

  return MetricCursor::openCursor(tsdb_.get(), metric, opts, cursor);
}

ReturnCode MetricService::insertSample(
    const MetricIDType& metric_id,
    const SeriesNameType& series_name,
    uint64_t time,
    const std::string& value) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ENOTFOUND", "metric not found");
  }

  std::shared_ptr<MetricSeries> series;
  auto rc = metric->getSeriesList()->findOrCreateSeries(
      tsdb_.get(),
      &id_provider_,
      metric_id,
      metric->getConfig(),
      series_name,
      &series);

  if (!rc.isSuccess()) {
    return rc;
  }

  tsdb::Cursor cursor;
  if (!tsdb_->getCursor(series->getSeriesID().id, &cursor, false)) {
    return ReturnCode::error("ERUNTIME", "can't open cursor");
  }

  auto input_aggregator = metric->getInputAggregator();
  if (!input_aggregator) {
    return ReturnCode::error("ERUNTIME", "can't open input aggregator");
  }

  tval_ref val;
  val.type = getMetricDataType(metric->getConfig()->kind);
  val.len = tval_len(val.type);
  val.data = alloca(val.len);

  int parse_rc = tval_fromstring(
      val.type,
      val.data,
      val.len,
      value.data(),
      value.size());

  if (!parse_rc) {
    return ReturnCode::errorf("ERUNTIME", "invalid value: '$0'", value);
  }

  rc = input_aggregator->addSample(
      &cursor,
      time,
      val.type,
      val.data,
      val.len);

  if (rc.isSuccess()) {
    tsdb_->commit(); // FIXME
  }

  return rc;
}

} // namsepace fnordmetric


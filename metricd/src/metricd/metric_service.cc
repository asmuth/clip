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
#include <iostream>
#include <sstream>
#include "metricd/metric_service.h"
#include "metricd/util/fileutil.h"
#include "metricd/util/logging.h"
#include "metricd/util/time.h"
#include "metricd/util/sha1.h"
#include "metricd/transport/statsd/statsd.h"

namespace fnordmetric {

static std::string getMetricFilePath(
    const std::string& datadir,
    const std::string& metric_id) {
  auto h = SHA1::compute(metric_id).toString();
  return FileUtil::joinPaths(datadir, "metrics/" + h.substr(0, 2) + "/" + h);
}

ReturnCode MetricService::startService(
    const std::string& datadir,
    std::unique_ptr<MetricService>* service) {
  if (!FileUtil::exists(datadir)) {
    return ReturnCode::errorf("EIO", "datadir doesn't exist: $0", datadir);
  }

  /* create metric directories */
  for (size_t i = 0; i < 256; ++i) {
    unsigned char c = i;
    auto dirname = "metrics/" + StringUtil::hexPrint(&c, 1);
    FileUtil::mkdir_p(FileUtil::joinPaths(datadir, dirname));
  }

  /* initialize service */
  service->reset(new MetricService(datadir));

  return ReturnCode::success();
}

ReturnCode MetricService::applyConfig(const ConfigList* config) {
  for (auto& mc : config->getMetricConfigs()) {
    configureMetric(mc.first, std::make_shared<MetricConfig>(mc.second));
  }

  return ReturnCode::success();
}

MetricService::MetricService(
    const std::string& datadir) :
    datadir_(datadir) {}

MetricListCursor MetricService::listMetrics() const {
  auto metric_map = metric_map_.getMetricMap();
  return MetricListCursor(metric_map);
}

ReturnCode MetricService::configureMetric(
    const std::string& metric_id,
    std::shared_ptr<MetricConfig> config) {
  std::unique_lock<std::mutex> lk(metric_map_mutex_);
  auto metric_map = metric_map_.getMetricMap();

  {
    auto metric = metric_map->findMetric(metric_id);
    if (metric) {
      logDebug("Updating metric config; metric_id=$0", metric_id);
      return metric->setConfig(config);
    }
  }

  std::unique_ptr<Metric> metric(new Metric(metric_id));
  logDebug("Creating metric; metric_id=$0", metric_id);
  auto rc = metric->setConfig(config);
  if (!rc.isSuccess()) {
    return rc;
  }

  MetricMapBuilder metric_map_builder(metric_map.get());
  metric_map_builder.addMetric(metric_id, std::move(metric));
  metric_map_.updateMetricMap(metric_map_builder.getMetricMap());

  return ReturnCode::success();
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

ReturnCode MetricService::fetchData(
    const MetricIDType& metric_id,
    const MetricCursorOptions& opts,
    MetricCursor* cursor) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ENOTFOUND", "metric not found");
  }

  //return MetricCursor::openCursor(tsdb_.get(), metric, opts, cursor);
  return ReturnCode::success();
}

ReturnCode MetricService::insertSample(
    const MetricIDType& metric_id,
    uint64_t time,
    const std::string& value) {
  auto metric_map = metric_map_.getMetricMap();
  auto metric = metric_map->findMetric(metric_id);
  if (!metric) {
    return ReturnCode::error("ENOTFOUND", "metric not found");
  }

  std::unique_lock<std::mutex> lk(metric->getInsertLock());
  auto metric_config = metric->getConfig();
  auto db_path = getMetricFilePath(datadir_, metric_id);

  /* open db file */
  std::unique_ptr<tsdb::TSDB> db;
  if (FileUtil::exists(db_path)) {
    if (!tsdb::TSDB::openDatabase(&db, db_path)) {
      return ReturnCode::errorf("EIO", "can't open database at $0", db_path);
    }
  } else {
    if (!tsdb::TSDB::createDatabase(&db, db_path)) {
      return ReturnCode::errorf("EIO", "can't create database at $0", db_path);
    }

    if (!db->createSeries(1, tval_len(getMetricDataType(metric_config->kind)), "")) {
      return ReturnCode::errorf("EIO", "can't create series in $0", db_path);
    }
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

  auto input_aggregator = mkInputAggregator(
      metric_config.get());

  if (!input_aggregator) {
    return ReturnCode::error("ERUNTIME", "can't open input aggregator");
  }

  tsdb::Cursor db_cursor;
  if (!db->getCursor(1, &db_cursor, false)) {
    return ReturnCode::error("EIO", "can't open tsdb cursor");
  }

  auto insert_rc = input_aggregator->addSample(
      &db_cursor,
      time,
      val.type,
      val.data,
      val.len);

  if (!insert_rc.isSuccess()) {
    return insert_rc;
  }

  db->commit(); // FIXME

  return ReturnCode::success();
}

MetricService::BatchInsertOptions::BatchInsertOptions() :
    metric_id_rewrite_enabled(false) {}

ReturnCode MetricService::insertSamplesBatch(
    const char* data,
    size_t len,
    const BatchInsertOptions* opts /* = nullptr */) {
  std::string metric_id;
  std::string series_id;
  std::string value;
  LabelSet labels;

  char const* cur = data;
  char const* end = data + len;

  while (cur < end) {
    if (!statsd::parseStatsdSample(&cur, end, &metric_id, &series_id, &value)) {
      return ReturnCode::error("EPARSE", "invalid packet");
    }

    if (opts && opts->metric_id_rewrite_enabled) {
      try {
        metric_id = std::regex_replace(
            metric_id,
            opts->metric_id_rewrite_regex,
            opts->metric_id_rewrite_replace,
            std::regex_constants::match_default |
            std::regex_constants::format_no_copy |
            std::regex_constants::format_first_only);

      } catch (const std::exception& e) {
        return ReturnCode::errorf("ERUNTIME", "regex error: $0", e.what());
      }
    }

    auto now = WallClock::unixMicros();
    auto rc = insertSample(
        metric_id,
        now,
        value);

    if (!rc.isSuccess()) {
      logWarning(
          "batch insert failed: $0; metric_id=$1 value=$2",
          rc.getMessage(),
          metric_id,
          value);
    }
  }

  return ReturnCode::success();
}

} // namsepace fnordmetric


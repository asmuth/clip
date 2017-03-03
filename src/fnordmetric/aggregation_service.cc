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
#include "fnordmetric/aggregation_service.h"
#include "fnordmetric/util/fileutil.h"
#include "fnordmetric/util/logging.h"
#include "fnordmetric/util/time.h"
#include "fnordmetric/util/sha1.h"
#include "fnordmetric/transport/statsd/statsd.h"

namespace fnordmetric {

ReturnCode AggregationService::startService(
    std::unique_ptr<AggregationService>* service) {
  /* initialize service */
  service->reset(new AggregationService());
  return ReturnCode::success();
}

AggregationService::AggregationService() {}

ReturnCode AggregationService::applyConfig(const ConfigList* config) {
  std::unique_lock<std::mutex> lk(table_map_mutex_);
  auto table_map = table_map_.getTableMap();
  TableMapBuilder table_map_builder(table_map.get());

  for (auto& mc : config->getTableConfigs()) {
    table_map_builder.addTable(mc.first, mc.second);
  }

  table_map_.updateTableMap(table_map_builder.getTableMap());
  return ReturnCode::success();
}

ReturnCode AggregationService::insertSample(const Sample& smpl) {
  return ReturnCode::success();
}

AggregationService::BatchInsertOptions::BatchInsertOptions() :
    metric_id_rewrite_enabled(false) {}

ReturnCode AggregationService::insertSamplesBatch(
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
    auto rc = insertSample(Sample(metric_id, value, now, {}));

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


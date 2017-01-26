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
#include <metricd/metric_cursor.h>
#include <metricd/util/logging.h>
#include <metricd/util/time.h>
#include <libtsdb/varint.h>

namespace fnordmetric {

MetricCursorOptions::MetricCursorOptions() {
  time_limit = WallClock::unixMicros();
  time_begin = time_limit - 2 * kMicrosPerHour;
  granularity = 0;
  align = 0;
  interpolate = true;
}

ReturnCode MetricCursor::openCursor(
    std::shared_ptr<const MetricConfig> metric_config,
    const std::string metric_file,
    const MetricCursorOptions& cursor_opts,
    MetricCursor* cursor) {
  std::unique_ptr<MetricCursorOptions> opts(
      new MetricCursorOptions(cursor_opts));

  /* fill in cursor options defaults from metric config */
  if (opts->granularity == 0) {
    opts->granularity = metric_config->display_granularity;
  }

  if (opts->granularity == 0) {
    opts->granularity = metric_config->granularity;
  }

  /* open db file */
  std::unique_ptr<tsdb::TSDB> db;
  if (!tsdb::TSDB::openDatabase(&db, metric_file)) {
    return ReturnCode::errorf("EIO", "can't open database at $0", metric_file);
  }

  tsdb::Cursor db_cursor;
  if (!db->getCursor(1, &db_cursor)) {
    return ReturnCode::error("EIO", "can't open tsdb cursor");
  }

  *cursor = MetricCursor(
      metric_config,
      std::move(opts),
      std::move(db),
      mkOutputAggregator(metric_config.get(), std::move(db_cursor), opts.get()));

  return ReturnCode::success();
}

MetricCursor::MetricCursor() {}

MetricCursor::MetricCursor(
    std::shared_ptr<const MetricConfig> config,
    std::unique_ptr<MetricCursorOptions> opts,
    std::unique_ptr<tsdb::TSDB> db,
    std::unique_ptr<OutputAggregator> reader) :
    config_(config),
    opts_(std::move(opts)),
    db_(std::move(db)),
    reader_(std::move(reader)) {}

MetricCursor::MetricCursor(
    MetricCursor&& o) :
    config_(std::move(o.config_)),
    opts_(std::move(o.opts_)),
    db_(std::move(o.db_)),
    reader_(std::move(o.reader_)){}

MetricCursor& MetricCursor::operator=(MetricCursor&& o) {
  config_ = std::move(o.config_);
  opts_ = std::move(o.opts_);
  db_ = std::move(o.db_);
  reader_ = std::move(o.reader_);
  return *this;
}

bool MetricCursor::next(uint64_t* timestamp, tval_ref* out) {
  return reader_->next(timestamp, out);
}

tval_type MetricCursor::getOutputType() const {
  return getOutputType(config_.get());
}

tval_type MetricCursor::getOutputType(const MetricConfig* config) {
  switch (config->kind) {

    case MetricKind::COUNTER_UINT64:
    case MetricKind::MAX_UINT64:
      return tval_type::UINT64;

    case MetricKind::COUNTER_INT64:
    case MetricKind::MAX_INT64:
      return tval_type::INT64;

    case MetricKind::COUNTER_FLOAT64:
    case MetricKind::MAX_FLOAT64:
      return tval_type::FLOAT64;

  }
}

std::unique_ptr<InputAggregator> mkInputAggregator(
    const MetricConfig* config) {
  switch (config->kind) {
    case MetricKind::MAX_UINT64:
    case MetricKind::MAX_INT64:
    case MetricKind::MAX_FLOAT64:
      return std::unique_ptr<InputAggregator>(
          new MaxInputAggregator(config->granularity));
    case MetricKind::COUNTER_UINT64:
    case MetricKind::COUNTER_INT64:
    case MetricKind::COUNTER_FLOAT64:
      return std::unique_ptr<InputAggregator>(
          new SumInputAggregator(config->granularity));
    default: return {};
  }
}

std::unique_ptr<OutputAggregator> mkOutputAggregator(
    const MetricConfig* config,
    tsdb::Cursor cursor,
    const MetricCursorOptions* cursor_opts) {
  switch (config->kind) {

    case MetricKind::MAX_UINT64:
    case MetricKind::MAX_INT64:
    case MetricKind::MAX_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new MaxOutputAggregator(
              std::move(cursor),
              getMetricDataType(config->kind),
              cursor_opts));

    case MetricKind::COUNTER_UINT64:
    case MetricKind::COUNTER_INT64:
    case MetricKind::COUNTER_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new SumOutputAggregator(
              std::move(cursor),
              getMetricDataType(config->kind),
              cursor_opts));

    default: return {};
  }
}

} // namespace fnordmetric


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
  cursor_type = MetricCursorType::SERIES;
  time_limit = WallClock::unixMicros();
  time_begin = time_limit - 2 * kMicrosPerHour;
  granularity = 0;
  align = 0;
  interpolate = true;
}

ReturnCode MetricCursor::openCursor(
    tsdb::TSDB* db,
    Metric* metric,
    const MetricCursorOptions& cursor_opts,
    MetricCursor* cursor) {
  std::unique_ptr<MetricCursorOptions> opts(
      new MetricCursorOptions(cursor_opts));

  /* fill in cursor options defaults from metric config */
  const auto& config = metric->getConfig();
  if (opts->granularity == 0) {
    opts->granularity = config.display_granularity;
  }

  if (opts->granularity == 0) {
    opts->granularity = config.granularity;
  }

  /* if no series id is given, perform the lookup */
  if (opts->series_id.id == 0) {
    std::shared_ptr<MetricSeries> series;
    if (!metric->getSeriesList()->findSeries(opts->series_name, &series)) {
      return ReturnCode::error("ENOTFOUND", "series not found");
    }

    opts->series_id = series->getSeriesID();
  }

  /* open tsdb cursors */
  std::vector<std::unique_ptr<OutputAggregator>> series_readers;
  {
    tsdb::Cursor tsdb_cursor;
    if (!db->getCursor(opts->series_id.id, &tsdb_cursor)) {
      return ReturnCode::error("EIO", "can't open tsdb cursor");
    }

    series_readers.emplace_back(
        mkOutputAggregator(
            config,
            std::move(tsdb_cursor),
            opts.get()));
  }

  *cursor = MetricCursor(
      metric->getConfig(),
      std::move(opts),
      std::move(series_readers));

  return ReturnCode::success();
}

MetricCursor::MetricCursor() {}

MetricCursor::MetricCursor(
    const MetricConfig& config,
    std::unique_ptr<MetricCursorOptions> opts,
    std::vector<std::unique_ptr<OutputAggregator>> series_readers)  :
    config_(config),
    opts_(std::move(opts)),
    series_readers_(std::move(series_readers)) {}

MetricCursor::MetricCursor(
    MetricCursor&& o) :
    config_(std::move(o.config_)),
    opts_(std::move(o.opts_)),
    series_readers_(std::move(o.series_readers_)) {
}

MetricCursor& MetricCursor::operator=(MetricCursor&& o) {
  config_ = std::move(o.config_);
  opts_ = std::move(o.opts_);
  series_readers_ = std::move(o.series_readers_);
  return *this;
}

bool MetricCursor::next(
    uint64_t* timestamp,
    tval_ref* out,
    size_t out_len) {
  return series_readers_[0]->next(timestamp, out, out_len);
}

tval_type MetricCursor::getOutputType() const {
  switch (config_.kind) {

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

size_t MetricCursor::getOutputColumnCount() const {
  return 1;
}

std::string MetricCursor::getOutputColumnName(size_t idx) const {
  assert(idx < 1);
  return "value";
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
    const MetricConfig& config,
    tsdb::Cursor cursor,
    const MetricCursorOptions* cursor_opts) {
  switch (config.kind) {

    case MetricKind::MAX_UINT64:
    case MetricKind::MAX_INT64:
    case MetricKind::MAX_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new MaxOutputAggregator(
              std::move(cursor),
              getMetricDataType(config.kind),
              cursor_opts));

    case MetricKind::COUNTER_UINT64:
    case MetricKind::COUNTER_INT64:
    case MetricKind::COUNTER_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new SumOutputAggregator(
              std::move(cursor),
              getMetricDataType(config.kind),
              cursor_opts));

    default: return {};
  }
}

} // namespace fnordmetric


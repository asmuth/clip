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

  /* open tsdb cursors */
  std::vector<std::unique_ptr<OutputAggregator>> series_readers;
  switch (opts->cursor_type) {

    case MetricCursorType::SERIES: {
      /* if no series id is given, perform the lookup */
      if (opts->series_id.id == 0) {
        std::shared_ptr<MetricSeries> series;
        if (!metric->getSeriesList()->findSeries(opts->series_name, &series)) {
          return ReturnCode::error("ENOTFOUND", "series not found");
        }

        opts->series_id = series->getSeriesID();
      }

      tsdb::Cursor tsdb_cursor;
      if (!db->getCursor(opts->series_id.id, &tsdb_cursor)) {
        return ReturnCode::error("EIO", "can't open tsdb cursor");
      }

      series_readers.emplace_back(
          mkOutputAggregator(
              config,
              std::move(tsdb_cursor),
              opts.get()));

      break;
    }

    case MetricCursorType::SUMMARY: {
      auto series_cursor = metric->getSeriesList()->listSeries();

      for (; series_cursor.isValid(); series_cursor.next()) {
        tsdb::Cursor tsdb_cursor;
        if (!db->getCursor(series_cursor.getSeriesID().id, &tsdb_cursor)) {
          return ReturnCode::error("EIO", "can't open tsdb cursor");
        }

        series_readers.emplace_back(
            mkOutputAggregator(
                config,
                std::move(tsdb_cursor),
                opts.get()));
      }

      break;
    }

  }

  /* set up group summary */
  std::unique_ptr<GroupSummary> group_summary;
  if (opts->cursor_type == MetricCursorType::SUMMARY) {
    group_summary.reset(
        new SumGroupSummary(getOutputType(metric->getConfig())));
  }

  *cursor = MetricCursor(
      metric->getConfig(),
      std::move(opts),
      std::move(series_readers),
      std::move(group_summary));

  return ReturnCode::success();
}

MetricCursor::MetricCursor() {}

MetricCursor::MetricCursor(
    const MetricConfig& config,
    std::unique_ptr<MetricCursorOptions> opts,
    std::vector<std::unique_ptr<OutputAggregator>> series_readers,
    std::unique_ptr<GroupSummary> group_summary) :
    config_(config),
    opts_(std::move(opts)),
    series_readers_(std::move(series_readers)),
    group_summary_(std::move(group_summary)) {}

MetricCursor::MetricCursor(
    MetricCursor&& o) :
    config_(std::move(o.config_)),
    opts_(std::move(o.opts_)),
    series_readers_(std::move(o.series_readers_)),
    group_summary_(std::move(o.group_summary_)) {}

MetricCursor& MetricCursor::operator=(MetricCursor&& o) {
  config_ = std::move(o.config_);
  opts_ = std::move(o.opts_);
  series_readers_ = std::move(o.series_readers_);
  group_summary_ = std::move(o.group_summary_);
  return *this;
}

bool MetricCursor::next(uint64_t* timestamp, tval_ref* out) {
  if (series_readers_.empty()) {
    return false;
  }

  switch (opts_->cursor_type) {

    case MetricCursorType::SERIES:
      return series_readers_[0]->next(timestamp, out);

    case MetricCursorType::SUMMARY: {
      tval_ref next_val;
      next_val.type = getOutputType();
      next_val.len = tval_len(next_val.type);
      next_val.data = alloca(next_val.len);

      for (auto& reader : series_readers_) {
        if (!reader->next(timestamp, &next_val)) {
          return false;
        }

        group_summary_->addValue(next_val.type, next_val.data, next_val.len);
      }

      if (out) {
        group_summary_->getValue(out[0].type, out[0].data, out[0].len);
      }

      group_summary_->reset();
      return true;
    }

  }
}

tval_type MetricCursor::getOutputType() const {
  return getOutputType(config_);
}

tval_type MetricCursor::getOutputType(const MetricConfig& config) {
  switch (config.kind) {

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

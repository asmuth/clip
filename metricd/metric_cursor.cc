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

MetricCursor::MetricCursor() {}

MetricCursor::MetricCursor(
    const MetricConfig* config,
    tsdb::Cursor cursor,
    std::unique_ptr<MetricCursorOptions> opts)  :
    config_(config),
    opts_(std::move(opts)),
    aggr_(mkOutputAggregator(std::move(cursor))) {}

MetricCursor::MetricCursor(
    MetricCursor&& o) :
    config_(o.config_),
    opts_(std::move(o.opts_)),
    aggr_(std::move(o.aggr_)) {
  o.config_ = nullptr;
}

MetricCursor& MetricCursor::operator=(MetricCursor&& o) {
  config_ = o.config_;
  o.config_ = nullptr;
  opts_ = std::move(o.opts_);
  aggr_ = std::move(o.aggr_);
  return *this;
}

bool MetricCursor::next(
    uint64_t* timestamp,
    tval_ref* out,
    size_t out_len) {
  if (aggr_) {
    return aggr_->next(timestamp, out, out_len);
  } else {
    return false;
  }
}

tval_type MetricCursor::getOutputType() const {
  if (aggr_) {
    return aggr_->getOutputType();
  } else {
    //return config_.data_type;
    return tval_type::UINT64; // FIXME
  }
}

size_t MetricCursor::getOutputColumnCount() const {
  if (aggr_) {
    return aggr_->getOutputColumnCount();
  } else {
    return 1;
  }
}

std::string MetricCursor::getOutputColumnName(size_t idx) const {
  if (aggr_) {
    return aggr_->getOutputColumnName(idx);
  } else {
    assert(idx < 1);
    return "value";
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

std::unique_ptr<OutputAggregator> MetricCursor::mkOutputAggregator(
    tsdb::Cursor cursor) {
  switch (config_->kind) {

    case MetricKind::MAX_UINT64:
    case MetricKind::MAX_INT64:
    case MetricKind::MAX_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new MaxOutputAggregator(
              std::move(cursor),
              getMetricDataType(config_->kind),
              opts_.get()));

    case MetricKind::COUNTER_UINT64:
    case MetricKind::COUNTER_INT64:
    case MetricKind::COUNTER_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new SumOutputAggregator(
              std::move(cursor),
              getMetricDataType(config_->kind),
              opts_.get()));

    default: return {};
  }
}

} // namespace fnordmetric


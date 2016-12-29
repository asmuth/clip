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
#include <libtsdb/varint.h>

namespace fnordmetric {

MetricCursor::MetricCursor() {}

MetricCursor::MetricCursor(
    const MetricConfig* config,
    tsdb::Cursor cursor,
    uint64_t time_begin,
    uint64_t time_limit) :
    cursor_(std::move(cursor)),
    aggr_(mkOutputAggregator(&cursor_, time_begin, time_limit, config)) {}

MetricCursor::MetricCursor(
    MetricCursor&& o) :
    cursor_(std::move(o.cursor_)),
    aggr_(std::move(o.aggr_)) {}

MetricCursor& MetricCursor::operator=(MetricCursor&& o) {
  cursor_ = std::move(o.cursor_);
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
  if (config->granularity == 0) {
    return {};
  }

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
    tsdb::Cursor* cursor,
    uint64_t time_begin,
    uint64_t time_limit,
    const MetricConfig* config) {
  uint64_t granularity = config->display_granularity;
  if (granularity == 0) {
    granularity = config->granularity;
  }

  if (granularity == 0) {
    return {};
  }

  switch (config->kind) {
    case MetricKind::MAX_UINT64:
    case MetricKind::MAX_INT64:
    case MetricKind::MAX_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new MaxOutputAggregator(
              cursor,
              getMetricDataType(config->kind),
              time_begin,
              time_limit,
              granularity));
    case MetricKind::COUNTER_UINT64:
    case MetricKind::COUNTER_INT64:
    case MetricKind::COUNTER_FLOAT64:
      return std::unique_ptr<OutputAggregator>(
          new SumOutputAggregator(
              cursor,
              getMetricDataType(config->kind),
              time_begin,
              time_limit,
              granularity));
    default: return {};
  }
}

} // namespace fnordmetric


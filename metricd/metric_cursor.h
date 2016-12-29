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
#pragma once
#include <metricd/metric.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <mutex>

namespace fnordmetric {
struct MetricConfig;

enum class MetricCursorType {
  SERIES,
  SUMMARY
};

struct MetricCursorOptions {

  MetricCursorOptions();

  MetricCursorType cursor_type;

  SeriesNameType series_name;
  SeriesIDType series_id;

  std::string series_name_match;


  /**
   * begin of scan time window (inclusive); unix microsecond timestamp;
   * default is now() - 2 hours
   */
  uint64_t time_begin;

  /**
   * end of scan time window (exclusive); unix microsecond timestamp;
   * default is now()
   */
  uint64_t time_limit;

  /**
   * unit: microseconds; default: 0 (zero means default granularity)
   */
  uint64_t granularity;

  uint64_t align;

  bool interpolate;

};

class MetricCursor {
public:

  static ReturnCode openCursor(
      tsdb::TSDB* db,
      Metric* metric,
      const MetricCursorOptions& cursor_opts,
      MetricCursor* cursor);

  MetricCursor();
  MetricCursor(
      const MetricConfig* config,
      tsdb::Cursor cursor,
      std::unique_ptr<MetricCursorOptions> opts);

  MetricCursor(const MetricCursor& o) = delete;
  MetricCursor(MetricCursor&& o);
  MetricCursor& operator=(const MetricCursor& o) = delete;
  MetricCursor& operator=(MetricCursor&& o);

  bool next(
      uint64_t* time,
      tval_ref* out,
      size_t out_len);

  tval_type getOutputType() const;

  size_t getOutputColumnCount() const;

  std::string getOutputColumnName(size_t idx) const;

protected:

  std::unique_ptr<OutputAggregator> mkOutputAggregator(tsdb::Cursor cursor);

  const MetricConfig* config_;
  std::unique_ptr<MetricCursorOptions> opts_;
  std::unique_ptr<OutputAggregator> aggr_;
};

std::unique_ptr<InputAggregator> mkInputAggregator(
    const MetricConfig* config);

} // namespace fnordmetric


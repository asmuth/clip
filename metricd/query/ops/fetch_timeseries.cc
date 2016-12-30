/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/query/ops/fetch_timeseries.h"

namespace fnordmetric {

FetchTimeseriesOperation::FetchTimeseriesOperation(
    MetricService* metric_service,
    const QueryOptions* query_opts) :
    metric_service_(metric_service),
    query_opts_(query_opts) {}

static void readDataFrame(MetricCursor* cursor, DataFrame* frame) {
  uint64_t timestamp;
  tval_ref val;
  val.type = cursor->getOutputType();
  val.len = tval_len(val.type);
  val.data = alloca(val.len);
  while (cursor->next(&timestamp, &val, 1)) {
    frame->addValue(timestamp, val.data, val.len);
  }
}

ReturnCode FetchTimeseriesOperation::execute(DataFrameBundle* out) {
  auto metric_id = query_opts_->getProperty("metric_id");
  if (!metric_id) {
    return ReturnCode::error("EARG", "missing argument: metric_id");
  }

  /* fetch summary */
  {
    MetricCursorOptions summary_cursor_opts;
    summary_cursor_opts.cursor_type = MetricCursorType::SUMMARY;

    MetricCursor cursor;

    auto cursor_rc = metric_service_->fetchData(
        *metric_id,
        summary_cursor_opts,
        &cursor);

    if (!cursor_rc.isSuccess()) {
      return cursor_rc;
    }

    auto frame = out->addFrame(cursor.getOutputType());
    frame->addTag("summary");
    readDataFrame(&cursor, frame);
  }

  /* fetch individual series */
  MetricSeriesListCursor list_cursor;
  auto rc = metric_service_->listSeries(*metric_id, &list_cursor);
  if (!rc.isSuccess()) {
    return rc;
  }

  for (; list_cursor.isValid(); list_cursor.next()) {
    MetricCursorOptions series_cursor_opts;
    series_cursor_opts.series_id = list_cursor.getSeriesID();

    MetricCursor cursor;
    auto cursor_rc = metric_service_->fetchData(
        *metric_id,
        series_cursor_opts,
        &cursor);

    if (!cursor_rc.isSuccess()) {
      return cursor_rc;
    }

    auto frame = out->addFrame(cursor.getOutputType());
    readDataFrame(&cursor, frame);
  }

  return ReturnCode::success();
}

} // namespace fnordmetric


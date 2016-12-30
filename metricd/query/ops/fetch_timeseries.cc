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
    const std::string& metric_id,
    MetricCursorOptions&& cursor_opts) :
    metric_service_(metric_service),
    metric_id_(metric_id),
    cursor_opts_(cursor_opts) {}

ReturnCode FetchTimeseriesOperation::execute(DataFrameBundle* out) {
  MetricCursor cursor;
  auto cursor_rc = metric_service_->fetchData(
      metric_id_,
      cursor_opts_,
      &cursor);

  if (!cursor_rc.isSuccess()) {
    return cursor_rc;
  }

  auto frame = out->addFrame(cursor.getOutputType());

  uint64_t timestamp;
  tval_ref val;
  val.type = cursor.getOutputType();
  val.len = tval_len(val.type);
  val.data = alloca(val.len);
  while (cursor.next(&timestamp, &val, 1)) {
    frame->addValue(timestamp, val.data, val.len);
  }

  return ReturnCode::success();
}

} // namespace fnordmetric


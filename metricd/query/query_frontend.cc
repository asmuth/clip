/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/query/query_frontend.h"

namespace fnordmetric {

QueryFrontend::QueryFrontend(
    MetricService* metric_service) :
    metric_service_(metric_service) {}

ReturnCode QueryFrontend::fetchTimeseriesJSON(
    const QueryOptions* query,
    json::JSONOutputStream* json) {

  auto metric_id = query->getProperty("metric_id");
  if (!metric_id) {
    return ReturnCode::error("EARG", "missing argument: metric_id");
  }

  DataFrameBundle results;

  /* fetch data */
  {
    MetricCursorOptions cursor_opts;
    //cursor_opts.cursor_type = MetricCursorType::SUMMARY;
    FetchTimeseriesOperation fetch_op(
        metric_service_,
        *metric_id,
        std::move(cursor_opts));

    auto rc = fetch_op.execute(&results);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  /* write output json */
  json->beginArray();

  for (size_t frame_idx = 0; frame_idx < results.getFrameCount(); ++frame_idx) {
    if (frame_idx > 0) { json->addComma(); }

    auto frame = results.getFrame(frame_idx);
    json->beginObject();
    json->addObjectEntry("series_id");
    json->addString(frame->getID());
    json->addComma();

    json->addObjectEntry("tags");
    json->beginArray();
    for (auto t = frame->getTags().begin(); t != frame->getTags().end(); ++t) {
      if (t != frame->getTags().begin()) { json->addComma(); }
      json->addString(*t);
    }
    json->endArray();
    json->addComma();

    json->addObjectEntry("time");
    json->beginArray();
    for (size_t i = 0; i < frame->getSize(); ++i) {
      if (i > 0) { json->addComma(); }
      json->addInteger(*frame->getTime(i));
    }
    json->endArray();
    json->addComma();

    json->addObjectEntry("values");
    json->beginArray();
    for (size_t i = 0; i < frame->getSize(); ++i) {
      if (i > 0) { json->addComma(); }
      switch (frame->getType()) {
        case tval_type::UINT64:
          json->addInteger(*((uint64_t*) frame->getData(i)));
          break;
        case tval_type::INT64:
          json->addInteger(*((int64_t*) frame->getData(i)));
          break;
        case tval_type::FLOAT64:
          json->addFloat(*((double*) frame->getData(i)));
          break;
        default:
          json->addNull();
      }
    }
    json->endArray();

    json->endObject();
  }

  json->endArray();

  return ReturnCode::success();
}

} // namespace fnordmetric


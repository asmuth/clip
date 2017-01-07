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

static void readDataFrame(MetricCursor* cursor, DataFrame* frame) {
  uint64_t timestamp;
  tval_ref val;
  val.type = cursor->getOutputType();
  val.len = tval_len(val.type);
  val.data = alloca(val.len);
  while (cursor->next(&timestamp, &val)) {
    frame->addValue(timestamp, val.data, val.len);
  }
}

ReturnCode QueryFrontend::fetchSummaryJSON(
    const json::JSONObject* req,
    json::JSONWriter* res) {
  MetricCursorOptions cursor_opts;
  cursor_opts.cursor_type = MetricCursorType::SUMMARY;

  auto metric_id = req->getString("metric_id");
  if (metric_id.empty()) {
    return ReturnCode::error("EARG", "missing argument: metric_id");
  }

  /* fetch summary */
  DataFrameBundle results;
  {
    MetricCursor cursor;
    auto cursor_rc = metric_service_->fetchData(
        metric_id,
        cursor_opts,
        &cursor);

    if (!cursor_rc.isSuccess()) {
      return cursor_rc;
    }

    auto frame = results.addFrame(cursor.getOutputType());
    readDataFrame(&cursor, frame);
  }

  /* compute gross summaries */
  std::vector<GrossSummaryMethod> summary_methods{GrossSummaryMethod::SUM};
  using SummaryList = std::vector<std::pair<std::string, tval_autoref>>;
  std::vector<SummaryList> gross_summaries(results.getFrameCount());
  for (size_t frame_idx = 0; frame_idx < results.getFrameCount(); ++frame_idx) {

    for (const auto& summary_method : summary_methods) {
      tval_autoref summary_val;
      auto rc = summarizeTimeseries(
          summary_method,
          results.getFrame(frame_idx),
          &summary_val);

      if (!rc.isSuccess()) {
        return rc;
      }

      gross_summaries[frame_idx].emplace_back(
          getGrossSummaryName(summary_method),
          std::move(summary_val));
    }
  }

  /* write output json */
  res->beginArray();

  for (size_t frame_idx = 0; frame_idx < results.getFrameCount(); ++frame_idx) {
    auto frame = results.getFrame(frame_idx);
    res->beginObject();
    res->addString("series_id");
    res->addString(frame->getID());

    res->addString("tags");
    res->beginArray();
    for (auto t = frame->getTags().begin(); t != frame->getTags().end(); ++t) {
      res->addString(*t);
    }
    res->endArray();

    res->addString("summaries");
    res->beginArray();
    for (size_t i = 0; i < gross_summaries[frame_idx].size(); ++i) {
      res->beginObject();
      res->addString("summary");
      res->addString(gross_summaries[frame_idx][i].first);

      res->addString("value");
      const auto& sval = gross_summaries[frame_idx][i].second.val;
      switch (sval.type) {
        case tval_type::UINT64:
          res->addInteger(*((uint64_t*) sval.data));
          break;
        case tval_type::INT64:
          res->addInteger(*((int64_t*) sval.data));
          break;
        case tval_type::FLOAT64:
          res->addFloat(*((double*) sval.data));
          break;
        default:
          res->addNull();
      }

      res->endObject();
    }
    res->endArray();

    res->addString("time");
    res->beginArray();
    for (size_t i = 0; i < frame->getSize(); ++i) {
      res->addInteger(*frame->getTime(i));
    }
    res->endArray();

    res->addString("values");
    res->beginArray();
    for (size_t i = 0; i < frame->getSize(); ++i) {
      switch (frame->getType()) {
        case tval_type::UINT64:
          res->addInteger(*((uint64_t*) frame->getData(i)));
          break;
        case tval_type::INT64:
          res->addInteger(*((int64_t*) frame->getData(i)));
          break;
        case tval_type::FLOAT64:
          res->addFloat(*((double*) frame->getData(i)));
          break;
        default:
          res->addNull();
      }
    }
    res->endArray();

    res->endObject();
  }

  res->endArray();

  return ReturnCode::success();
}

} // namespace fnordmetric


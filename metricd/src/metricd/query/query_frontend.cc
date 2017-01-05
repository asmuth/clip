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
    json::JSONWriter* json) {
  std::vector<GrossSummaryMethod> summary_methods{GrossSummaryMethod::SUM};

  /* fetch series */
  DataFrameBundle results;
  {
    FetchTimeseriesOperation fetch_op(metric_service_, query);
    auto rc = fetch_op.execute(&results);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  /* compute gross summaries */
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
  json->beginArray();

  for (size_t frame_idx = 0; frame_idx < results.getFrameCount(); ++frame_idx) {
    auto frame = results.getFrame(frame_idx);
    json->beginObject();
    json->addString("series_id");
    json->addString(frame->getID());

    json->addString("tags");
    json->beginArray();
    for (auto t = frame->getTags().begin(); t != frame->getTags().end(); ++t) {
      json->addString(*t);
    }
    json->endArray();

    json->addString("summaries");
    json->beginArray();
    for (size_t i = 0; i < gross_summaries[frame_idx].size(); ++i) {
      json->beginObject();
      json->addString("summary");
      json->addString(gross_summaries[frame_idx][i].first);

      json->addString("value");
      const auto& sval = gross_summaries[frame_idx][i].second.val;
      switch (sval.type) {
        case tval_type::UINT64:
          json->addInteger(*((uint64_t*) sval.data));
          break;
        case tval_type::INT64:
          json->addInteger(*((int64_t*) sval.data));
          break;
        case tval_type::FLOAT64:
          json->addFloat(*((double*) sval.data));
          break;
        default:
          json->addNull();
      }

      json->endObject();
    }
    json->endArray();

    json->addString("time");
    json->beginArray();
    for (size_t i = 0; i < frame->getSize(); ++i) {
      json->addInteger(*frame->getTime(i));
    }
    json->endArray();

    json->addString("values");
    json->beginArray();
    for (size_t i = 0; i < frame->getSize(); ++i) {
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


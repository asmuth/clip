/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include "metricd/query/query_frontend.h"
#include "metricd/util/format.h"

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
    if (val.type == tval_type::NIL) {
      frame->setNullFlag(frame->getSize() - 1, true);
    }
  }
}

static void writeTValToJSON(
    const tval_ref* val,
    json::JSONWriter* json) {
  switch (val->type) {
    case tval_type::UINT64:
      assert(val->len == sizeof(uint64_t));
      json->addInteger(*((uint64_t*) val->data));
      break;
    case tval_type::INT64:
      assert(val->len == sizeof(int64_t));
      json->addInteger(*((int64_t*) val->data));
      break;
    case tval_type::FLOAT64:
      assert(val->len == sizeof(double));
      json->addFloat(*((double*) val->data));
      break;
    default:
      json->addNull();
  }
}

static void writeUnitConfigToJSON(
    const MetricInfo* metric_info,
    json::JSONWriter* json) {
  auto metric_config = metric_info->getMetricConfig();
  auto unit_config = metric_info->getUnitConfig();
  if (!unit_config) {
    json->addNull();
    return;
  }

  json->beginObject();
  json->addString("unit_id");
  json->addString(unit_config->unit_id);
  json->addString("description");
  json->addString(unit_config->description);

  if (metric_config->unit_scale.val.type != tval_type::NIL) {
    json->addString("unit_scale");
    writeTValToJSON(&metric_config->unit_scale.val, json);
  }

  json->addString("names");
  json->beginObject();
  for (const auto& n : unit_config->names) {
    json->addString(n.first);
    json->beginObject();
    json->addString("singular");
    json->addString(n.second.singular);
    json->addString("plural");
    json->addString(n.second.plural);
    json->addString("symbol");
    json->addString(n.second.symbol);
    json->addString("factor");
    writeTValToJSON(&n.second.factor.val, json);
    json->endObject();
  }
  json->endObject();

  json->endObject();
}

ReturnCode getGrossSummaryMethodsFromJSON(
    const json::JSONElement* param,
    std::vector<GrossSummaryMethod>* summary_methods) {
  std::vector<std::string> method_strs;
  if (param->isString()) {
    auto method_str = param->getAsString()->getString();
    for (const auto& s : StringUtil::split(method_str, ",")) {
      method_strs.emplace_back(s);
    }
  }

  if (param->isArray()) {
    auto method_arr = param->getAsArray();
    for (size_t i = 0; i < method_arr->size(); ++i) {
      auto elem = method_arr->get(i);
      if (!elem->isString()) {
        continue;
      }

      method_strs.emplace_back(elem->getAsString()->getString());
    }
  }

  for (const auto& method_str : method_strs) {
    GrossSummaryMethod method;
    if (!getGrossSummaryFromName(&method, method_str)) {
      return ReturnCode::errorf(
          "EARG",
          "invalid gross summary method: $0",
          method_str);
    }

    summary_methods->emplace_back(method);
  }

  return ReturnCode::success();
}

ReturnCode QueryFrontend::fetchSeriesJSON(
    const json::JSONObject* req,
    json::JSONWriter* res) {
  /* get metric info */
  auto metric_id = req->getString("metric_id");
  if (metric_id.empty()) {
    return ReturnCode::error("EARG", "missing argument: metric_id");
  }

  MetricInfo metric_info;
  auto describe_rc = metric_service_->describeMetric(metric_id, &metric_info);
  if (!describe_rc.isSuccess()) {
    return describe_rc;
  }

  /* configure time range */
  MetricCursorOptions cursor_opts;
  auto time_begin = req->getString("from");
  if (!time_begin.empty()) {
    auto rc = parseTimeSpec(time_begin, &cursor_opts.time_begin);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  auto time_limit = req->getString("until");
  if (!time_limit.empty()) {
    auto rc = parseTimeSpec(time_limit, &cursor_opts.time_limit);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  /* configure summary methods */
  std::vector<GrossSummaryMethod> summary_methods;
  auto summary_methods_param = req->get("summarize_gross");
  if (summary_methods_param) {
    auto rc = getGrossSummaryMethodsFromJSON(
        summary_methods_param,
        &summary_methods);

    if (!rc.isSuccess()) {
      return rc;
    }
  } else {
    summary_methods = metric_info.getMetricConfig()->summarize_gross;
  }

  /* fetch series */
  DataFrameBundle results;
  MetricSeriesListCursor list_cursor;
  auto rc = metric_service_->listSeries(metric_id, &list_cursor);
  if (!rc.isSuccess()) {
    return rc;
  }

  for (; list_cursor.isValid(); list_cursor.next()) {
    MetricCursorOptions series_cursor_opts;
    series_cursor_opts.series_id = list_cursor.getSeriesID();

    MetricCursor cursor;
    auto cursor_rc = metric_service_->fetchData(
        metric_id,
        series_cursor_opts,
        &cursor);

    if (!cursor_rc.isSuccess()) {
      return cursor_rc;
    }

    auto frame = results.addFrame(cursor.getOutputType());
    frame->setID(list_cursor.getSeriesName().name);
    readDataFrame(&cursor, frame);
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
  res->beginObject();
  res->addString("unit");
  writeUnitConfigToJSON(&metric_info, res);

  res->addString("series");
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
      if (frame->getNullFlag(i)) {
        res->addNull();
        continue;
      }

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
  res->endObject();

  return ReturnCode::success();
}

ReturnCode QueryFrontend::fetchSummaryJSON(
    const json::JSONObject* req,
    json::JSONWriter* res) {
  /* get metric info */
  auto metric_id = req->getString("metric_id");
  if (metric_id.empty()) {
    return ReturnCode::error("EARG", "missing argument: metric_id");
  }

  MetricInfo metric_info;
  auto describe_rc = metric_service_->describeMetric(metric_id, &metric_info);
  if (!describe_rc.isSuccess()) {
    return describe_rc;
  }

  auto metric_config = metric_info.getMetricConfig();

  /* configure time range */
  MetricCursorOptions cursor_opts;
  cursor_opts.cursor_type = MetricCursorType::SUMMARY;
  auto time_begin = req->getString("from");
  if (!time_begin.empty()) {
    auto rc = parseTimeSpec(time_begin, &cursor_opts.time_begin);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  auto time_limit = req->getString("until");
  if (!time_limit.empty()) {
    auto rc = parseTimeSpec(time_limit, &cursor_opts.time_limit);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  /* configure gross summary methods */
  std::vector<GrossSummaryMethod> summary_methods;
  auto summary_methods_param = req->get("summarize_gross");
  if (summary_methods_param) {
    auto rc = getGrossSummaryMethodsFromJSON(
        summary_methods_param,
        &summary_methods);

    if (!rc.isSuccess()) {
      return rc;
    }
  } else {
    summary_methods = metric_info.getMetricConfig()->summarize_gross;
  }

  /* configure group summary method */
  cursor_opts.summarize_group = metric_config->summarize_group.get();
  auto summary_method_group_param = req->getString("summarize_group");
  if (!summary_method_group_param.empty()) {
    auto rc = getGroupSummaryFromName(
        &cursor_opts.summarize_group,
        summary_method_group_param);

    if (!rc) {
      return ReturnCode::errorf(
          "EARG",
          "invalid group summary: $0",
          summary_method_group_param);
    }
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
  res->beginObject();
  res->addString("unit");
  writeUnitConfigToJSON(&metric_info, res);

  res->addString("series");
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
      if (frame->getNullFlag(i)) {
        res->addNull();
        continue;
      }

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
  res->endObject();

  return ReturnCode::success();
}

} // namespace fnordmetric


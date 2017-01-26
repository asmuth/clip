/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <metricd/metric_service.h>
#include <metricd/query/query_frontend.h>
#include <metricd/transport/http/http_api.h>
#include <metricd/util/stringutil.h>
#include <metricd/util/time.h>
#include "metricd/util/format.h"
#include <libtransport/json/json_object.h>

namespace fnordmetric {

HTTPAPI::HTTPAPI(
    MetricService* metric_service) :
    metric_service_(metric_service),
    query_frontend_(metric_service) {}

void HTTPAPI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  URI uri(request->uri());
  auto path = uri.path();
  StringUtil::stripTrailingSlashes(&path);

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "GET, POST");
  response->addHeader("Access-Control-Allow-Headers", "Authorization");

  if (request->method() == http::HTTPMessage::M_OPTIONS) {
    response->setStatus(http::kStatusOK);
    return;
  }

  // PATH: /api/v1/metrics/list
  if (path == "/api/v1/metrics/list") {
    renderMetricList(request, response, uri);
    return;
  }

  // PATH: /api/v1/metrics/fetch_timeseries
  if (path == "/api/v1/metrics/fetch_timeseries") {
    performMetricFetchTimeseries(request, response, uri);
    return;
  }

  // PATH: /api/v1/metrics/fetch_summary
  if (path == "/api/v1/metrics/fetch_summary") {
    performMetricFetchSummary(request, response, uri);
    return;
  }

  // PATH: /api/v1/metrics/insert
  if (path == "/api/v1/metrics/insert") {
    performMetricInsert(request, response, uri);
    return;
  }

  response->setStatus(http::kStatusNotFound);
  response->addBody("not found");
}

void HTTPAPI::renderMetricList(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  std::string json_str;
  json::JSONWriter json(&json_str);

  json.beginObject();
  json.addString("metrics");
  json.beginArray();

  auto cursor = metric_service_->listMetrics();
  for (; cursor.isValid(); cursor.next()) {
    json.beginObject();

    json.addString("metric_id");
    json.addString(cursor.getMetricID());

    json.endObject();
  }

  json.endArray();
  json.endObject();

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->addBody(json_str);
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

void HTTPAPI::performMetricFetchTimeseries(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  /* read params */
  json::JSONObjectStorage json_req;
  if (request->method() == http::HTTPMessage::M_POST) {
    const auto& body = request->body();
    if (!json::readJSON(&json_req, &body) || !json_req.hasRootObject()) {
      response->setStatus(http::kStatusBadRequest);
      response->addBody("ERROR: invalid json");
      return;
    }
  } else {
    auto params = uri.queryParams();

    std::string metric_id;
    if (URI::getParam(params, "metric_id", &metric_id)) {
      json_req.getRootAsObject()->setString("metric_id", metric_id);
    }

    std::string from;
    if (URI::getParam(params, "from", &from)) {
      json_req.getRootAsObject()->setString("from", from);
    }

    std::string until;
    if (URI::getParam(params, "until", &until)) {
      json_req.getRootAsObject()->setString("until", until);
    }

    std::string summarize;
    if (URI::getParam(params, "summarize", &summarize)) {
      json_req.getRootAsObject()->setString("summarize", summarize);
    }
  }

  /* configure cursor */
  auto metric_id = json_req->getString("metric_id");
  MetricCursorOptions cursor_opts;
  if (metric_id.empty()) {
    response->setStatus(http::kStatusBadRequest);
    response->addBody("missing argument: metric_id");
    return;
  }

  /* configure time range */
  auto time_begin = json_req->getString("from");
  if (!time_begin.empty()) {
    auto rc = parsePointInTime(time_begin, &cursor_opts.time_begin);
    if (!rc.isSuccess()) {
      response->setStatus(http::kStatusBadRequest);
      response->addBody("missing argument: metric_id");
      return;
    }
  }

  auto time_limit = json_req->getString("until");
  if (!time_limit.empty()) {
    auto rc = parsePointInTime(time_limit, &cursor_opts.time_limit);
    if (!rc.isSuccess()) {
      response->setStatus(http::kStatusBadRequest);
      response->addBody("missing argument: metric_id");
      return;
    }
  }

  /* get metric info */
  MetricInfo metric_info;
  auto describe_rc = metric_service_->describeMetric(metric_id, &metric_info);
  if (!describe_rc.isSuccess()) {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody(std::string("ERROR: ") + describe_rc.getMessage());
    return;
  }

  /* configure summary methods */
  std::vector<GrossSummaryMethod> summary_methods;
  auto summary_methods_param = json_req->get("summarize");
  if (summary_methods_param) {
    auto rc = getGrossSummaryMethodsFromJSON(
        summary_methods_param,
        &summary_methods);

    if (!rc.isSuccess()) {
      response->setStatus(http::kStatusBadRequest);
      response->addBody(std::string("ERROR: ") + rc.getMessage());
      return;
    }
  } else {
    summary_methods = metric_info.getMetricConfig()->summarize_gross;
  }

  /* fetch series */
  MetricCursor cursor;
  auto cursor_rc = metric_service_->fetchData(
      metric_id,
      cursor_opts,
      &cursor);

  if (!cursor_rc.isSuccess()) {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody(std::string("ERROR: ") + cursor_rc.getMessage());
    return;
  }

  DataFrame result(cursor.getOutputType());
  readDataFrame(&cursor, &result);

  /* compute gross summaries */
  std::vector<std::pair<std::string, tval_autoref>> summaries;
  for (const auto& summary_method : summary_methods) {
    tval_autoref summary_val;
    auto rc = summarizeTimeseries(summary_method, &result, &summary_val);

    if (!rc.isSuccess()) {
      response->setStatus(http::kStatusInternalServerError);
      response->addBody(std::string("ERROR: ") + rc.getMessage());
      return;
    }

    summaries.emplace_back(
        getGrossSummaryName(summary_method),
        std::move(summary_val));
  }

  /* write output json */
  std::string json_res_str;
  json::JSONWriter json_res(&json_res_str);

  json_res.beginObject();
  json_res.addString("unit");
  writeUnitConfigToJSON(&metric_info, &json_res);

  json_res.addString("summaries");
  json_res.beginArray();
  for (size_t i = 0; i < summaries.size(); ++i) {
    json_res.beginObject();
    json_res.addString("summary");
    json_res.addString(summaries[i].first);

    json_res.addString("value");
    const auto& sval = summaries[i].second.val;
    switch (sval.type) {
      case tval_type::UINT64:
        json_res.addInteger(*((uint64_t*) sval.data));
        break;
      case tval_type::INT64:
        json_res.addInteger(*((int64_t*) sval.data));
        break;
      case tval_type::FLOAT64:
        json_res.addFloat(*((double*) sval.data));
        break;
      default:
        json_res.addNull();
    }

    json_res.endObject();
  }
  json_res.endArray();

  json_res.addString("timeseries");
  json_res.beginObject();
  json_res.addString("time");
  json_res.beginArray();
  for (size_t i = 0; i < result.getSize(); ++i) {
    json_res.addInteger(*result.getTime(i));
  }
  json_res.endArray();
  json_res.addString("values");
  json_res.beginArray();
  for (size_t i = 0; i < result.getSize(); ++i) {
    if (result.getNullFlag(i)) {
      json_res.addNull();
      continue;
    }

    switch (result.getType()) {
      case tval_type::UINT64:
        json_res.addInteger(*((uint64_t*) result.getData(i)));
        break;
      case tval_type::INT64:
        json_res.addInteger(*((int64_t*) result.getData(i)));
        break;
      case tval_type::FLOAT64:
        json_res.addFloat(*((double*) result.getData(i)));
        break;
      default:
        json_res.addNull();
    }
  }
  json_res.endArray();
  json_res.endObject();

  json_res.endObject();

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->addBody(json_res_str);
}

void HTTPAPI::performMetricFetchSummary(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  std::string json_res_str;
  json::JSONWriter json_res(&json_res_str);

  json_res.beginObject();
  json_res.endObject();

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->addBody(json_res_str);
}

void HTTPAPI::performMetricInsert(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  // FIXME check POST

  auto params = uri.queryParams();

  std::string metric_id;
  if (!URI::getParam(params, "metric_id", &metric_id)) {
    response->addBody("error: missing ?metric_id=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::string value;
  if (!URI::getParam(params, "value", &value)) {
    response->addBody("error: missing ?value=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  auto now = WallClock::unixMicros();
  auto rc = metric_service_->insertSample(
      metric_id,
      now,
      value);

  if (rc.isSuccess()) {
    response->setStatus(http::kStatusCreated);
  } else {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody("ERROR: " + rc.getMessage());
  }
}

} // namespace fnordmetric


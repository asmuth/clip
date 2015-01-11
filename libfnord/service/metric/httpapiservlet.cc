/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/service/metric/httpapiservlet.h"

namespace fnord {
namespace metric_service {

HTTPAPIServlet::HTTPAPIServlet(
    MetricService* metric_service) :
    metric_service_(metric_service) {}

void HTTPAPIServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  URI uri(req->uri());

  //try {
    if (StringUtil::endsWith(uri.path(), "/insert")) {
      return insertSample(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/list")) {
      return listMetrics(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/query")) {
      return query(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/chart")) {
      res->setStatus(fnord::http::kStatusOK);
      res->addBody("chart");
      return;
    }

    res->setStatus(fnord::http::kStatusNotFound);
    res->addBody("not found");
  //} catch (const std::exception& e) {
  //  res->setStatus(http::kStatusInternalServerError);
  //  res->addBody(StringUtil::format("error: $0", e.what()));
 // }
}

void HTTPAPIServlet::listMetrics(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream jsons(response->getBodyOutputStream());

  jsons.beginObject();
  jsons.addObjectEntry("metrics");
  jsons.beginArray();

  fnord::URI::ParamList params = uri->queryParams();
  std::string filter_query;
  auto filter_enabled = fnord::URI::getParam(params, "filter", &filter_query);

  int limit = -1;
  std::string limit_string;
  if (fnord::URI::getParam(params, "limit", &limit_string)) {
    limit = std::stoi(limit_string);
  }

  int i = 0;
  for (const auto& metric : metric_service_->listMetrics()) {
    if (filter_enabled &&
        metric->key().find(filter_query) == std::string::npos) {
      continue;
    }

    if (i++ > 0) {
      jsons.addComma();
    }

    renderMetricJSON(metric, &jsons);

    if (limit > 0 && i == limit) {
      break;
    }
  }

  jsons.endArray();
  jsons.endObject();
}

void HTTPAPIServlet::insertSample(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  const auto& postbody = request->body();
  fnord::URI::ParamList params;

  if (postbody.size() > 0) {
    fnord::URI::parseQueryString(postbody.toString(), &params);
  } else {
    params = uri->queryParams();
  }

  std::string metric_key;
  if (!fnord::URI::getParam(params, "metric", &metric_key)) {
    response->addBody("error: invalid metric key: " + metric_key);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::string value_str;
  if (!fnord::URI::getParam(params, "value", &value_str)) {
    response->addBody("error: missing ?value=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::vector<std::pair<std::string, std::string>> labels;
  static const char kLabelParamPrefix[] = "label[";
  for (const auto& param : params) {
    const auto& key = param.first;
    const auto& value = param.second;

    if (key.compare(0, sizeof(kLabelParamPrefix) - 1, kLabelParamPrefix) == 0 &&
        key.back() == ']') {
      auto label_key = key.substr(
          sizeof(kLabelParamPrefix) - 1,
          key.size() - sizeof(kLabelParamPrefix));

      labels.emplace_back(label_key, value);
    }
  }

  double sample_value;
  try {
    sample_value = std::stod(value_str);
  } catch (std::exception& e) {
    response->addBody("error: invalid value: " + value_str);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  metric_service_->insertSample(metric_key, sample_value, labels);
  response->setStatus(http::kStatusCreated);
  response->addBody("ok");
}

void HTTPAPIServlet::query(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  auto params = uri->queryParams();

  DateTime from = DateTime::epoch();
  DateTime until;

  Vector<ScopedPtr<Query>> queries;
  queries.emplace_back(new Query());

  for (const auto param : params) {

    // param: metric
    if (param.first == "metric") {
      if (queries.back()->metric_key.length() > 0) {
        queries.emplace_back(new Query());
      }

      queries.back()->metric_key = param.second;
      continue;
    }

    // param: aggr_fn
    if (param.first == "aggr_fn") {
      queries.back()->aggr_fn = Query::aggrFnFromString(param.second);
      continue;
    }

    // param: aggr_window
    if (param.first == "aggr_window") {
      queries.back()->aggr_window =
          Duration(std::stod(param.second) * kMicrosPerSecond);
      continue;
    }

    // param: aggr_step
    if (param.first == "aggr_step") {
      queries.back()->aggr_step =
          Duration(std::stod(param.second) * kMicrosPerSecond);
      continue;
    }

    // param: group_by
    if (param.first == "group_by") {
      queries.back()->group_by.emplace_back(param.second);
      continue;
    }

    // param: join_with
    if (param.first == "join_with") {
      queries.back()->join_metric_key = param.second;
      continue;
    }

    // param: join_fn
    if (param.first == "join_fn") {
      queries.back()->join_fn = Query::joinFnFromString(param.second);
      continue;
    }

    // param: join_aggr_fn
    if (param.first == "join_aggr_fn") {
      queries.back()->join_aggr_fn = Query::aggrFnFromString(param.second);
      continue;
    }

    // param: scale
    if (param.first == "scale") {
      queries.back()->scale = std::stod(param.second);
      continue;
    }

    // param: from
    if (param.first == "from") {
      //from = DateTime::fromTimespec(param.second);
      continue;
    }

    // param: until
    if (param.first == "until") {
      //until = DateTime::fromTimespec(param.second);
      continue;
    }

    RAISEF(kParseError, "invalid param: $0", param.first);
  }

  for (const auto& query : queries) {
    std::vector<Query::ResultRowType> results;
    query->run(from, until, metric_service_, &results);
  }

  response->setStatus(http::kStatusCreated);
  response->addBody("ok");
/*
  query::QueryService::kFormat resp_format = query::QueryService::FORMAT_JSON;
  std::string format_param;
  if (fnord::URI::getParam(params, "format", &format_param)) {
    if (format_param == "svg") {
      resp_format = query::QueryService::FORMAT_SVG;
    }
  }
*/
}

void HTTPAPIServlet::renderMetricJSON(
    fnord::metric_service::IMetric* metric,
    fnord::json::JSONOutputStream* json) const {
  json->beginObject();

  json->addObjectEntry("key");
  json->addString(metric->key());
  json->addComma();

  json->addObjectEntry("total_bytes");
  json->addInteger(metric->totalBytes());
  json->addComma();

  json->addObjectEntry("last_insert");
  json->addInteger(static_cast<uint64_t>(metric->lastInsertTime()));
  json->addComma();

  json->addObjectEntry("labels");
  json->beginArray();
  auto labels = metric->labels();
  for (auto cur = labels.begin(); cur != labels.end(); ++cur) {
    if (cur != labels.begin()) {
      json->addComma();
    }
    json->addString(*cur);
  }
  json->endArray();

  json->endObject();
}

}
}

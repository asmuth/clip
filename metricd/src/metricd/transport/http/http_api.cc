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
#include <metricd/metric_service.h>
#include <metricd/query/query_frontend.h>
#include <metricd/transport/http/http_api.h>
#include <metricd/util/stringutil.h>
#include <metricd/util/time.h>
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

  // PATH: /api/v1/metrics/list_series
  if (path == "/api/v1/metrics/list_series") {
    renderMetricSeriesList(request, response, uri);
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

void HTTPAPI::renderMetricSeriesList(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
  auto params = uri.queryParams();

  std::string metric_id;
  if (!URI::getParam(params, "metric_id", &metric_id)) {
    response->setStatus(http::kStatusBadRequest);
    response->addBody("ERROR: missing parameter ?metric_id=...");
    return;
  }

  MetricSeriesListCursor cursor;
  auto rc = metric_service_->listSeries(metric_id, &cursor);
  if (!rc.isSuccess()) {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody("ERROR: " + rc.getMessage());
    return;
  }

  std::string json_str;
  json::JSONWriter json(&json_str);

  json.beginObject();
  json.addString("metric_id");
  json.addString(metric_id);
  json.addString("series");
  json.beginArray();

  for (; cursor.isValid(); cursor.next()) {
    json.beginObject();

    json.addString("series_id");
    json.addString(cursor.getSeriesName().name);

    json.endObject();
  }

  json.endArray();
  json.endObject();

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  response->addBody(json_str);
}

void HTTPAPI::performMetricFetchSummary(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    const URI& uri) {
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
  }

  std::string json_res_str;
  json::JSONWriter json_res(&json_res_str);

  auto rc = query_frontend_.fetchSummaryJSON(
      json_req.getRootAsObject(),
      &json_res);

  if (rc.isSuccess()) {
    response->setStatus(http::kStatusOK);
    response->addHeader("Content-Type", "application/json; charset=utf-8");
    response->addBody(json_res_str);
  } else {
    response->setStatus(http::kStatusInternalServerError);
    response->addBody("ERROR: " + rc.getMessage());
  }
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

  std::string series_id;
  URI::getParam(params, "series_id", &series_id);

  std::string value;
  if (!URI::getParam(params, "value", &value)) {
    response->addBody("error: missing ?value=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  auto now = WallClock::unixMicros();
  auto rc = metric_service_->insertSample(
      metric_id,
      SeriesNameType(series_id),
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


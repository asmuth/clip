/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/stringutil.h>
#include <fnord-metricdb/metricrepository.h>
#include "environment.h"
#include "queryendpoint.h"
#include "chartsql/queryservice.h"
#include "sql/backends/metricservice/metrictablerepository.h"
#include "sql/backends/csv/csvbackend.h"
#include "sql/backends/mysql/mysqlbackend.h"

namespace fnordmetric {

static const char kMetricsUrl[] = "/metrics";
static const char kMetricsUrlPrefix[] = "/metrics/";
static const char kQueryUrl[] = "/query";
static const char kLabelParamPrefix[] = "label[";

QueryEndpoint::QueryEndpoint(
    fnord::metric_service::IMetricRepository* metric_repo) :
    metric_repo_(metric_repo) {}

void QueryEndpoint::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  fnord::URI uri(request->uri());
  auto path = uri.path();
  fnord::StringUtil::stripTrailingSlashes(&path);

  response->addHeader("Access-Control-Allow-Origin", "*");

  auto params = uri.queryParams();

  std::shared_ptr<fnord::InputStream> input_stream;
  std::string get_query;
  if (fnord::URI::getParam(params, "q", &get_query)) {
    input_stream.reset(new fnord::StringInputStream(get_query));
  } else {
    input_stream = request->getBodyInputStream();
  }

  std::shared_ptr<fnord::OutputStream> output_stream =
      response->getBodyOutputStream();

  query::QueryService query_service;
  std::unique_ptr<query::TableRepository> table_repo(
      new query::MetricTableRepository(metric_repo_));

  if (!env()->flags()->isSet("disable_external_sources")) {
    query_service.registerBackend(
        std::unique_ptr<fnordmetric::query::Backend>(
            new fnordmetric::query::mysql_backend::MySQLBackend));

    query_service.registerBackend(
        std::unique_ptr<fnordmetric::query::Backend>(
            new fnordmetric::query::csv_backend::CSVBackend));
  }

  query::QueryService::kFormat resp_format = query::QueryService::FORMAT_JSON;
  std::string format_param;
  if (fnord::URI::getParam(params, "format", &format_param)) {
    if (format_param == "svg") {
      resp_format = query::QueryService::FORMAT_SVG;
    }
  }

  response->setStatus(http::kStatusOK);

  switch (resp_format) {
    case query::QueryService::FORMAT_JSON:
      response->addHeader("Content-Type", "application/json; charset=utf-8");
      break;
    case query::QueryService::FORMAT_SVG:
      response->addHeader("Content-Type", "text/html; charset=utf-8");
      break;
    default:
      break;
  }

  int width = -1;
  std::string width_param;
  if (fnord::URI::getParam(params, "width", &width_param)) {
    width = std::stoi(width_param);
  }

  int height = -1;
  std::string height_param;
  if (fnord::URI::getParam(params, "height", &height_param)) {
    height = std::stoi(height_param);
  }

  try {
    query_service.executeQuery(
        input_stream,
        resp_format,
        output_stream,
        std::move(table_repo),
        width,
        height);

  } catch (fnord::Exception e) {
    response->clearBody();

    fnord::json::JSONOutputStream json(std::move(output_stream));
    json.beginObject();
    json.addObjectEntry("status");
    json.addString("error");
    json.addComma();
    json.addObjectEntry("error");
    json.addString(e.getMessage());
    json.endObject();
  }

}

}

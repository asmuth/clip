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
#include <fnordmetric/environment.h>
#include <fnordmetric/httpapi.h>
#include <fnordmetric/chartsql/queryservice.h>
#include <fnordmetric/sql/backends/metricservice/metrictablerepository.h>
#include <fnordmetric/sql/backends/csv/csvbackend.h>
#include <fnordmetric/sql/backends/mysql/mysqlbackend.h>

namespace fnordmetric {

static const char kMetricsUrl[] = "/metrics";
static const char kMetricsUrlPrefix[] = "/metrics/";
static const char kQueryUrl[] = "/query";
static const char kLabelParamPrefix[] = "label[";

HTTPAPI::HTTPAPI(
    fnord::metric_service::IMetricRepository* metric_repo) :
    metric_repo_(metric_repo) {}

bool HTTPAPI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  fnord::URI uri(request->getUrl());
  auto path = uri.path();
  fnord::StringUtil::stripTrailingSlashes(&path);

  response->addHeader("Access-Control-Allow-Origin", "*");

  // PATH: ^/metrics/?$
  if (path == kMetricsUrl) {
    switch (request->method()) {
      case http::HTTPRequest::M_GET:
        renderMetricList(request, response, &uri);
        return true;
      case http::HTTPRequest::M_POST:
        insertSample(request, response, &uri);
        return true;
      default:
        return false;
    }
  }

  // PATH: ^/metrics/.*
  if (path.compare(0, sizeof(kMetricsUrlPrefix) - 1, kMetricsUrlPrefix) == 0) {
    // PATH: ^/metrics/(.*)$
    switch (request->method()) {
      case http::HTTPRequest::M_GET:
        renderMetricSampleScan(request, response, &uri);
        return true;
      default:
        return false;
    }
  }

  // PATH: ^/query/?*
  if (path == kQueryUrl) {
    switch (request->method()) {
      case http::HTTPRequest::M_GET:
      case http::HTTPRequest::M_POST:
        executeQuery(request, response, &uri);
        return true;
      default:
        return false;
    }
    return true;
  }

  return false;
}

void HTTPAPI::renderMetricList(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    fnord::URI* uri) {
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
    try {
      limit = std::stoi(limit_string);
    } catch (const std::exception& e) {
      /* fallthrough */
    }
  }

  int i = 0;
  for (const auto& metric : metric_repo_->listMetrics()) {
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

void HTTPAPI::insertSample(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    fnord::URI* uri) {
  const auto& postbody = request->getBody();
  fnord::URI::ParamList params;

  if (postbody.size() > 0) {
    fnord::URI::parseQueryString(postbody, &params);
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

  auto metric = metric_repo_->findOrCreateMetric(metric_key);
  metric->insertSample(sample_value, labels);
  response->setStatus(http::kStatusCreated);
}

void HTTPAPI::renderMetricSampleScan(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    fnord::URI* uri) {
  auto metric_key = uri->path().substr(sizeof(kMetricsUrlPrefix) - 1);
  if (metric_key.size() < 3) {
    response->addBody("error: invalid metric key: " + metric_key);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  auto metric = metric_repo_->findMetric(metric_key);
  if (metric == nullptr) {
    response->addBody("metric not found: " + metric_key);
    response->setStatus(http::kStatusNotFound);
    return;
  }

  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream jsons(response->getBodyOutputStream());

  jsons.beginObject();

  jsons.addObjectEntry("metric");
  renderMetricJSON(metric, &jsons);
  jsons.addComma();

  jsons.addObjectEntry("samples");
  jsons.beginArray();

  int i = 0;
  metric->scanSamples(
      fnord::DateTime::epoch(),
      fnord::DateTime::now(),
      [&jsons, &i] (fnord::metric_service::Sample* sample) -> bool {
        if (i++ > 0) { jsons.addComma(); }
        jsons.beginObject();

        jsons.addObjectEntry("time");
        jsons.addValue<uint64_t>(static_cast<uint64_t>(sample->time()));
        jsons.addComma();

        jsons.addObjectEntry("value");
        jsons.addValue<double>(sample->value());
        jsons.addComma();

        jsons.addObjectEntry("labels");
        jsons.beginObject();
        auto labels = sample->labels();
        for (int n = 0; n < labels.size(); n++) {
          if (n > 0) {
            jsons.addComma();
          }

          jsons.addObjectEntry(labels[n].first);
          jsons.addString(labels[n].second);
        }
        jsons.endObject();

        jsons.endObject();
        return true;
      });

  jsons.endArray();
  jsons.endObject();
}

void HTTPAPI::executeQuery(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    fnord::URI* uri) {
  auto params = uri->queryParams();

  std::shared_ptr<io::InputStream> input_stream;
  std::string get_query;
  if (fnord::URI::getParam(params, "q", &get_query)) {
    input_stream.reset(new fnord::io::StringInputStream(get_query));
  } else {
    input_stream = request->getBodyInputStream();
  }

  std::shared_ptr<io::OutputStream> output_stream =
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

void HTTPAPI::renderMetricJSON(
    fnord::metric_service::IMetric* metric,
    fnord::json::JSONOutputStream* json) const {
  json->beginObject();

  json->addObjectEntry("key");
  json->addString(metric->key());
  json->addComma();

  json->addObjectEntry("total_bytes");
  json->addValue<size_t>(metric->totalBytes());
  json->addComma();

  json->addObjectEntry("last_insert");
  json->addValue<uint64_t>(static_cast<uint64_t>(metric->lastInsertTime()));
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

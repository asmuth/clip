/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/httpapi.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/metricdb/metrictablerepository.h>
#include <fnordmetric/util/stringutil.h>

namespace fnordmetric {
namespace metricdb {

static const char kMetricsUrl[] = "/metrics";
static const char kMetricsUrlPrefix[] = "/metrics/";
static const char kQueryUrl[] = "/query";
static const char kLabelParamPrefix[] = "label[";

HTTPAPI::HTTPAPI(MetricRepository* metric_repo) : metric_repo_(metric_repo) {}

bool HTTPAPI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  util::URI uri(request->getUrl());
  auto path = uri.path();
  fnord::util::StringUtil::stripTrailingSlashes(&path);

  // PATH: ^/metrics/?$
  if (path == kMetricsUrl) {
    switch (request->method()) {
      case http::HTTPRequest::M_GET:
        renderMetricList(request, response, &uri);
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
      case http::HTTPRequest::M_POST:
        insertSample(request, response, &uri);
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
    util::URI* uri) {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  util::JSONOutputStream json(response->getBodyOutputStream());

  json.beginObject();
  json.addObjectEntry("metrics");
  json.beginArray();

  int i = 0;
  for (const auto& metric : metric_repo_->listMetrics()) {
    if (i++ > 0) { json.addComma(); }
    renderMetricJSON(metric, &json);
  }

  json.endArray();
  json.endObject();
}

void HTTPAPI::insertSample(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    util::URI* uri) {
  auto params = uri->queryParams();

  auto metric_key = uri->path().substr(sizeof(kMetricsUrlPrefix) - 1);
  if (metric_key.size() < 3) {
    response->addBody("error: invalid metric key: " + metric_key);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::string value_str;
  if (!util::URI::getParam(params, "value", &value_str)) {
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

  Sample<double> sample;
  sample.labels = std::move(labels);
  try {
    sample.value = std::stod(value_str);
  } catch (std::exception& e) {
    response->addBody("error: invalid value: " + value_str);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  auto metric = metric_repo_->findOrCreateMetric(metric_key);
  metric->addSample(sample);
  response->setStatus(http::kStatusCreated);
}

void HTTPAPI::renderMetricSampleScan(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    util::URI* uri) {
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
  util::JSONOutputStream json(response->getBodyOutputStream());

  json.beginObject();

  json.addObjectEntry("metric");
  renderMetricJSON(metric, &json);
  json.addComma();

  json.addObjectEntry("samples");
  json.beginArray();

  int i = 0;
  metric->scanSamples(
      fnord::util::DateTime::epoch(),
      fnord::util::DateTime::now(),
      [&json, &i] (MetricCursor* cursor) -> bool {
        auto sample = cursor->sample<double>();
        if (i++ > 0) { json.addComma(); }
        json.beginObject();

        json.addObjectEntry("time");
        json.addLiteral<uint64_t>(cursor->time());
        json.addComma();

        json.addObjectEntry("value");
        json.addLiteral<double>(sample->value());
        json.addComma();

        json.addObjectEntry("labels");
        json.beginObject();
        auto labels = sample->labels();
        for (int n = 0; n < labels.size(); n++) {
          if (n > 0) {
            json.addComma();
          }

          json.addObjectEntry(labels[n].first);
          json.addString(labels[n].second);
        }
        json.endObject();

        json.endObject();
        return true;
      });

  json.endArray();
  json.endObject();
}

void HTTPAPI::executeQuery(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    util::URI* uri) {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");

  std::shared_ptr<util::InputStream> input_stream =
      request->getBodyInputStream();

  std::shared_ptr<util::OutputStream> output_stream =
      response->getBodyOutputStream();

  // FIXPAUL move to thread/worker pool
  query::QueryService query_service;
  std::unique_ptr<query::TableRepository> table_repo(
      new MetricTableRepository(metric_repo_));

  try {
    query_service.executeQuery(
        input_stream,
        query::QueryService::FORMAT_JSON,
        output_stream,
        std::move(table_repo));

  } catch (util::RuntimeException e) {
    response->clearBody();

    util::JSONOutputStream json(std::move(output_stream));
    json.beginObject();
    json.addObjectEntry("status");
    json.addString("error");
    json.addComma();
    json.addObjectEntry("error");
    json.addString(e.getMessage());
    json.endObject();
  }

  response->addHeader(
      "Content-Length",
      std::to_string(response->getBody().size()));
}


void HTTPAPI::renderMetricJSON(
    Metric* metric,
    util::JSONOutputStream* json) const {
  json->beginObject();
  json->addObjectEntry("key");
  json->addString(metric->key());
  json->addComma();
  json->addObjectEntry("total_bytes");
  json->addLiteral<size_t>(metric->totalBytes());
  json->endObject();
}

}
}

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
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/util/jsonoutputstream.h>
#include <fnordmetric/util/stringutil.h>
#include <fnordmetric/util/uri.h>

namespace fnordmetric {
namespace metricdb {

static const char kMetricsUrl[] = "/metrics";
static const char kMetricsUrlPrefix[] = "/metrics/";
static const char kQueryUrl[] = "/query";

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
        renderMetricList(request, response);
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
        return true;
      case http::HTTPRequest::M_POST:
        insertSample(request, response);
        return true;
      default:
        return false;
    }
  }

  // PATH: ^/query/?*
  if (path == kQueryUrl) {
    switch (request->method()) {
      case http::HTTPRequest::M_GET:
        return true;
      case http::HTTPRequest::M_POST:
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
    http::HTTPResponse* response) {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  util::JSONOutputStream json(response->getBodyOutputStream());

  json.beginObject();
  json.addObjectEntry("metrics");
  json.beginArray();

  for (const auto& metric : metric_repo_->listMetrics()) {

  }

  json.endArray();
  json.endObject();
}

void HTTPAPI::insertSample(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  response->setStatus(http::kStatusCreated);
  response->addBody("CREATED");
}



}
}

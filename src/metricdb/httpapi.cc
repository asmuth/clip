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

namespace fnordmetric {
namespace metricdb {

HTTPAPI::HTTPAPI(MetricRepository* metric_repo) : metric_repo_(metric_repo) {}

bool HTTPAPI::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  auto url = request->getUrl();

  if (url == "/metrics") {
    response->setStatus(200);
    response->addHeader("Content-Type", "application/json; charset=utf-8");
    util::JSONOutputStream json(response->getBodyOutputStream());

    json.beginObject();
    json.addObjectEntry("metrics");
    json.beginArray();

    for (const auto& metri : metric_repo_->listMetrics()) {

    }

    json.endArray();
    json.endObject();
    return true;
  }

  return false;
}

}
}

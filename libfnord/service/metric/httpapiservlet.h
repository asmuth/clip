/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRIC_SERVICE_HTTPAPISERVLET_H
#define _FNORD_METRIC_SERVICE_HTTPAPISERVLET_H
#include "fnord/net/http/httpservice.h"
#include "fnord/service/metric/metricservice.h"
#include "fnord/service/metric/query.h"
#include "fnord/json/json.h"

namespace fnord {
namespace metric_service {

class HTTPAPIServlet : public fnord::http::HTTPService {
public:
  HTTPAPIServlet(MetricService* metric_service);

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res);

protected:

  void insertSample(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void listMetrics(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void query(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void renderMetricJSON(
      fnord::metric_service::IMetric* metric,
      json::JSONOutputStream* json) const;

  MetricService* metric_service_;
};

}
}
#endif

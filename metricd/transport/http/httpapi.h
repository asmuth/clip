/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <metricd/transport/http/httpservice.h>
#include <metricd/query/query_frontend.h>
#include <metricd/util/uri.h>

namespace fnordmetric {
class MetricService;

class HTTPAPI : public http::HTTPService {
public:

  HTTPAPI(MetricService* metric_service);

  void handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response) override;

protected:

  void renderMetricList(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  void renderMetricSeriesList(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  void performMetricFetch(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  void performMetricInsert(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  MetricService* metric_service_;
  QueryFrontend query_frontend_;
};

}


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <fnordmetric/webui/webui.h>
#include <libtransport/http/v1/http_server.h>
#include <libtransport/http/http_request.h>
#include <libtransport/http/http_response.h>
#include <libtransport/uri/uri.h>
#include <libtransport/json/json.h>
#include <libtransport/json/json_reader.h>
#include <libtransport/json/json_writer.h>

namespace fnordmetric {
class AggregationService;

namespace json = libtransport::json;
namespace http = libtransport::http;

class HTTPAPI {
public:

  HTTPAPI(AggregationService* aggr_service);

  void handleHTTPRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

protected:

  void renderMetricList(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  void performMetricFetchTimeseries(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  void performMetricFetchSummary(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  void performMetricInsert(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      const URI& uri);

  AggregationService* aggr_service_;
};


class HTTPServer {
public:

  HTTPServer(
      AggregationService* aggr_service,
      const std::string& asset_path);

  bool listen(const std::string& addr, int port);
  bool run();

protected:

  void handleRequest(
      http::HTTPRequest* request,
      http::HTTPResponse* response);

  HTTPAPI http_api_;
  WebUI webui_;
  libtransport::http::HTTPServer http_server_;
};


}


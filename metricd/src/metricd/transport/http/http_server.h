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
#include <metricd/webui/webui.h>
#include <metricd/transport/http/http_api.h>
#include <libtransport/http/v1/http_server.h>

namespace fnordmetric {
class MetricService;

class HTTPServer {
public:

  HTTPServer(
      MetricService* metric_service,
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


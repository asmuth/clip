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
#include <metricd/transport/http/http_server.h>

namespace fnordmetric {

HTTPServer::HTTPServer(
    MetricService* metric_service,
    const std::string& asset_path) :
    http_api_(metric_service),
    webui_(asset_path) {
  http_server_.setRequestHandler(
      std::bind(
          &HTTPServer::handleRequest,
          this,
          std::placeholders::_1,
          std::placeholders::_2));
}

bool HTTPServer::listen(const std::string& addr, int port) {
  return http_server_.listen(addr, port);
}

bool HTTPServer::run() {
  return http_server_.run();
}

void HTTPServer::handleRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {

  if (StringUtil::beginsWith(request->uri(), "/api/")) {
    http_api_.handleHTTPRequest(request, response);
    return;
  }

  webui_.handleHTTPRequest(request, response);
}

}


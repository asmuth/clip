/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <thread>
#include <libtransport/http/v1/http_server.h>

void handleRequest(
    libtransport::http::HTTPRequest* request,
    libtransport::http::HTTPResponse* response) {
  std::cerr << "Got request: " <<  request->uri() << std::endl;;

  response->setStatus(200, "OK");
  response->addBody("hello world\n");
}

void handleConnection(std::unique_ptr<libtransport::http::HTTPConnection> conn) {
  auto c = conn.release();

  auto t = std::thread([c] {
    libtransport::http::HTTPServer::handleConnection(
      std::unique_ptr<libtransport::http::HTTPConnection>(c),
      std::bind(
          &handleRequest,
          std::placeholders::_1,
          std::placeholders::_2));
  });

  t.detach();
}

int main() {
  libtransport::http::HTTPServer server;
  server.setConnectionHandler(
      std::bind(&handleConnection, std::placeholders::_1));

  server.listen("localhost", 8080);
  server.run();
  return 0;
}


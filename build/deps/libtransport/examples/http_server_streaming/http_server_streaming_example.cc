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
#include <unistd.h>
#include <libtransport/http/v1/http_server.h>

void handleConnection(std::unique_ptr<libtransport::http::HTTPConnection> conn) {
  auto c = conn.release();

  auto t = std::thread([c] {
    std::unique_ptr<libtransport::http::HTTPConnection> connection(c);

    libtransport::http::HTTPRequest request;
    if (!connection->recvRequest(&request)) {
      return;
    }

    libtransport::http::HTTPResponse response;
    response.populateFromRequest(request);
    response.setStatus(200, "OK");
    response.addHeader("Content-Length", "300");

    connection->sendResponseHeaders(response);

    for (int i = 0; i < 50; ++i) {
      connection->sendResponseBodyChunk("fnord\n", 6);
      usleep(200000);
    }
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


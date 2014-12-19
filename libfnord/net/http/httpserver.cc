/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/exception.h>
#include <fnord/base/inspect.h>
#include <fnord/base/wallclock.h>
#include <fnord/net/http/httpconnection.h>
#include <fnord/net/http/httpserver.h>

using fnord::Exception;
using fnord::io::FileInputStream;
using fnord::io::FileOutputStream;

namespace fnord {
namespace http {

HTTPServer::HTTPServer(
    TaskScheduler* server_scheduler,
    TaskScheduler* request_scheduler) :
    ssock_(server_scheduler),
    server_scheduler_(server_scheduler),
    request_scheduler_(request_scheduler),
    enable_keepalive_(false),
    logger_(log::Logger::get()) {
  ssock_.onConnection([this] (std::unique_ptr<net::TCPConnection> conn) {
    HTTPConnection::start(
        std::move(conn),
        server_scheduler_,
        [this] (HTTPConnection* conn, HTTPRequest* req) {
            this->dispatchRequest(conn, req);
        });
  });
}

void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

void HTTPServer::listen(int port) {
  logger_->logf(fnord::log::kNotice, "Starting HTTP server on port $0", port);
  ssock_.listen(port);
}

void HTTPServer::dispatchRequest(HTTPConnection* conn, HTTPRequest* req) {
  HTTPResponse response;
  response.populateFromRequest(*req);
  response.setStatus(kStatusNotFound);
  response.addBody("Not Found");

  conn->writeResponseHeaders(
      response,
      std::bind(&HTTPConnection::finishResponse, conn));
}

}
}

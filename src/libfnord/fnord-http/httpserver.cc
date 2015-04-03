/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/exception.h>
#include <fnord-base/inspect.h>
#include <fnord-base/logging.h>
#include <fnord-base/wallclock.h>
#include "fnord-http/httpserverconnection.h"
#include <fnord-http/httpserver.h>

/*
TODO:
  - timeouts
  - httpconnection -> httpserverconnection
  - eventloop
  - 100 continue
  - chunked encoding
  - https
*/

namespace fnord {
namespace http {

HTTPServer::HTTPServer(
    HTTPHandlerFactory* handler_factory,
    TaskScheduler* scheduler) :
    handler_factory_(handler_factory),
    scheduler_(scheduler),
    ssock_(scheduler) {
  ssock_.onConnection([this] (std::unique_ptr<net::TCPConnection> conn) {
    HTTPServerConnection::start(
        handler_factory_,
        std::move(conn),
        scheduler_,
        &stats_);
  });
}

void HTTPServer::listen(int port) {
  logNotice("fnord.http.server", "Starting HTTP server on port $0", port);
  ssock_.listen(port);
}

HTTPServerStats* HTTPServer::stats() {
  return &stats_;
}

}
}

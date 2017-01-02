/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/util/exception.h>
#include <metricd/util/inspect.h>
#include <metricd/util/logging.h>
#include <metricd/util/time.h>
#include "metricd/transport/http/httpserverconnection.h"
#include <metricd/transport/http/httpserver.h>

namespace fnordmetric {
namespace http {

HTTPServer::HTTPServer(
    HTTPHandlerFactory* handler_factory,
    TaskScheduler* scheduler) :
    handler_factory_(handler_factory),
    scheduler_(scheduler),
    ssock_(scheduler) {
  ssock_.onConnection([this] (std::unique_ptr<TCPConnection> conn) {
    HTTPServerConnection::start(
        handler_factory_,
        std::move(conn),
        scheduler_,
        &stats_);
  });
}

void HTTPServer::listen(int port) {
  logNotice("Starting HTTP server on port $0", port);
  ssock_.listen(port);
}

HTTPServerStats* HTTPServer::stats() {
  return &stats_;
}

}
}

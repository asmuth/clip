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
    HTTPHandlerFactory* handler_factory,
    TaskScheduler* server_scheduler,
    TaskScheduler* request_scheduler) :
    handler_factory_(handler_factory),
    server_scheduler_(server_scheduler),
    request_scheduler_(request_scheduler),
    ssock_(server_scheduler) {
  ssock_.onConnection([this] (std::unique_ptr<net::TCPConnection> conn) {
    HTTPConnection::start(
        handler_factory_,
        std::move(conn),
        server_scheduler_);
  });
}

void HTTPServer::listen(int port) {
  log::Logger::get()->logf(
      fnord::log::kNotice,
      "Starting HTTP server on port $0",
      port);

  ssock_.listen(port);
}

}
}

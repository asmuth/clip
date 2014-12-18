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
    this->handleConnection(std::move(conn));
  });
}

void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

void HTTPServer::listen(int port) {
  logger_->logf(fnord::log::kNotice, "Starting HTTP server on port $0", port);
  ssock_.listen(port);
}

void HTTPServer::handleConnection(
    std::unique_ptr<net::TCPConnection> conn) const {
  new HTTPConnection(
      std::move(conn),
      server_scheduler_,
      request_scheduler_);

/*
  FileInputStream input_stream(fd, true);
  FileOutputStream output_stream(fd, true);

  do {
    HTTPRequest request;
    HTTPResponse response;

    keepalive = false;
    try {
      HTTPInputStream http_input_stream(&input_stream);
      request.readFromInputStream(&http_input_stream);

      if (request.keepalive()) {
        keepalive = enable_keepalive_;
      }

      response.populateFromRequest(request);
    } catch (fnord::Exception e) {
      keepalive = false;
      response.setStatus(kStatusNotFound);
      response.addHeader("Connection", "close");
      response.addBody("Bad Request");
      e.debugPrint(); // FIXPAUL
    }

    bool handled = false;
    try {
      for (const auto& handler : handlers_) {
        if (handler->handleHTTPRequest(&request, &response)) {
          handled = true;
          break;
        }
      }
    } catch (fnord::Exception e) {
      keepalive = false;
      response.setStatus(kStatusInternalServerError);
      response.addHeader("Connection", "close");
      response.addBody("Internal Server Error");
      e.debugPrint(); // FIXPAUL
    }

    if (!handled) {
      response.setStatus(kStatusNotFound);
      response.addBody("Not Found");
    }

    HTTPOutputStream http_output_stream(&output_stream);
    response.writeToOutputStream(&http_output_stream);
  } while (keepalive);
*/
  // FIXPAUL close connection
}

}
}

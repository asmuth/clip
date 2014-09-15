/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/http/httpserver.h>
#include <fnordmetric/http/httpinputstream.h>
#include <fnordmetric/http/httpoutputstream.h>
#include <fnordmetric/http/httprequest.h>
#include <fnordmetric/http/httpresponse.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace http {

void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

ThreadedHTTPServer::ThreadedHTTPServer(
    util::ThreadPool* thread_pool) :
    thread_pool_(thread_pool) {}

void ThreadedHTTPServer::onConnection(int fd) {
  thread_pool_->run([this, fd] () {
    handleConnection(fd);
  });
}

void ThreadedHTTPServer::handleConnection(int fd) const {
  bool keepalive = false;

  util::FileInputStream input_stream(fd, false);
  util::FileOutputStream output_stream(fd, true);

  do {
    HTTPRequest request;
    HTTPResponse response;

    keepalive = false;
    try {
      HTTPInputStream http_input_stream(&input_stream);
      request.readFromInputStream(&http_input_stream);

      if (request.keepalive()) {
        keepalive = true;
      }

      response.populateFromRequest(request);
    } catch (util::RuntimeException e) {
      keepalive = false;
      response.setStatus(400);
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
    } catch (util::RuntimeException e) {
      keepalive = false;
      response.setStatus(500);
      response.addHeader("Connection", "close");
      response.addBody("Internal Server Error");
      e.debugPrint(); // FIXPAUL
    }

    if (!handled) {
      response.setStatus(404);
      response.addBody("Not Found");
    }

    HTTPOutputStream http_output_stream(&output_stream);
    response.writeToOutputStream(&http_output_stream);
  } while (keepalive);
}

}
}

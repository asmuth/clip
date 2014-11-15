/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/http/httpinputstream.h>
#include <fnordmetric/http/httpoutputstream.h>
#include <fnordmetric/http/httpserver.h>
#include <fnordmetric/http/httprequest.h>
#include <fnordmetric/http/httpresponse.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/wallclock.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

using fnordmetric::util::FileInputStream;
using fnordmetric::util::FileOutputStream;
using fnordmetric::util::RuntimeException;

namespace fnord {
namespace http {

HTTPServer::HTTPServer(
    TaskScheduler* server_scheduler,
    TaskScheduler* request_scheduler) :
    server_scheduler_(server_scheduler),
    request_scheduler_(request_scheduler) {}


void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

void HTTPServer::listen(int port) {
  ssock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_ == 0) {
    RAISE(kIOError, "create socket() failed");
  }

  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (::bind(ssock_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    RAISE_ERRNO(kIOError, "bind() failed");
  }

  if (::listen(ssock_, 1024) == -1) {
    RAISE_ERRNO(kIOError, "listen() failed");
    return;
  }

  accept();
  //server_scheduler_->run(
  //    thread::Task::create(std::bind(&HTTPServer::accept, this)));
}

void HTTPServer::accept() {
  for (;;) {
    int conn_fd = ::accept(ssock_, NULL, NULL);

    if (conn_fd < 0) {
      RAISE_ERRNO(kIOError, "accept() failed");
    }

    request_scheduler_->run(
        thread::Task::create(
            std::bind(&HTTPServer::handleConnection, this, conn_fd)));
  }
}

void HTTPServer::handleConnection(int fd) const {
  if (fnordmetric::env()->verbose()) {
    fnordmetric::env()->logger()->printf(
        "DEBUG",
        "New HTTP connection on fd %i",
        fd);
  }

  bool keepalive = false;

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
        keepalive = true;
      }

      response.populateFromRequest(request);
    } catch (RuntimeException e) {
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
    } catch (RuntimeException e) {
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
}

}
}

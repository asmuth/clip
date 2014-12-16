/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fnord/base/exception.h>
#include <fnord/base/wallclock.h>
#include <fnord/net/http/httpinputstream.h>
#include <fnord/net/http/httpoutputstream.h>
#include <fnord/net/http/httpserver.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>

using fnord::Exception;
using fnord::io::FileInputStream;
using fnord::io::FileOutputStream;

namespace fnord {
namespace http {

HTTPServer::HTTPServer(
    TaskScheduler* server_scheduler,
    TaskScheduler* request_scheduler) :
    server_scheduler_(server_scheduler),
    request_scheduler_(request_scheduler),
    enable_keepalive_(false),
    logger_(log::Logger::get()) {}


void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

void HTTPServer::listen(int port) {
  logger_->logf(fnord::log::kNotice, "Starting HTTP server on port $0", port);

  ssock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_ == 0) {
    RAISE(kIOError, "create socket() failed");
  }

  int opt = 1;
  if (setsockopt(ssock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    RAISE_ERRNO(kIOError, "setsockopt(SO_REUSEADDR) failed");
    return;
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
        std::bind(&HTTPServer::handleConnection, this, conn_fd));
  }
}

void HTTPServer::handleConnection(int fd) const {
  logger_->logf(fnord::log::kDebug, "New HTTP connection on fd $0", fd);

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

  // FIXPAUL close connection
}

}
}

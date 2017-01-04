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
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "libtransport/http/v1/http_server.h"

namespace libtransport {
namespace http {

HTTPServer::HTTPServer() : ssock_(-1), io_timeout_(5000000) {
  setRequestHandler([] (HTTPRequest* request, HTTPResponse* response) {
    response->setStatus(kStatusNotFound);
    response->addBody("not found\n");
  });
}

bool HTTPServer::listen(const std::string& listen_addr, int listen_port) {
  ssock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_ == 0) {
    return false;
  }

  int opt = 1;
  if (setsockopt(ssock_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    return false;
  }

  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); // FIXMER
  addr.sin_port = htons(listen_port);
  if (::bind(ssock_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    return false;
  }

  if (::listen(ssock_, 1024) == -1) {
    return false;
  }

  return true;
}

bool HTTPServer::run() {
  for (;;) {
    int conn_fd = ::accept(ssock_, NULL, NULL);
    if (!conn_fd) {
      return false;
    }

    std::unique_ptr<HTTPConnection> conn(new HTTPConnection(
        conn_fd,
        io_timeout_));

    connection_handler_(std::move(conn));
  }

  return true;
}

void HTTPServer::handleConnection(
    std::unique_ptr<HTTPConnection> connection,
    RequestCallbackType request_handler) {
  HTTPRequest request;
  if (!connection->recvRequest(&request)) {
    return;
  }

  HTTPResponse response;
  response.populateFromRequest(request);
  request_handler(&request, &response);

  connection->sendResponse(response);
}

void HTTPServer::setConnectionHandler(ConnectionCallbackType callback) {
  connection_handler_ = callback;
}

void HTTPServer::setRequestHandler(RequestCallbackType cb) {
  connection_handler_ = std::bind(
      &HTTPServer::handleConnection,
      std::placeholders::_1,
      cb);
}

}
}


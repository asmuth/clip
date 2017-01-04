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
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include "libtransport/http/v1/http_client.h"

namespace libtransport {
namespace http {

HTTPClient::HTTPClient() : io_timeout_(5000000) {}

bool HTTPClient::connect(const std::string& hostname, uint16_t port) {
  struct hostent* h = gethostbyname(hostname.c_str());
  if (h == NULL) {
    error_ = "gethostbyname() failed";
    return false;
  }

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    error_ = "socket() failed";
    return false;
  }

  size_t nodelay = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  memcpy(&(saddr.sin_addr), h->h_addr, sizeof(saddr.sin_addr));
  memset(&(saddr.sin_zero), 0, 8);

  int rc = ::connect(
      fd,
      (const struct sockaddr *) &saddr,
      sizeof(saddr));

  if (rc < 0) {
    close(fd);
    error_ = "connect() failed";
    return false;
  }

  conn_.reset(new HTTPConnection(fd, io_timeout_));
  return true;
}

bool HTTPClient::executeRequest(const HTTPRequest& req, HTTPResponse* res) {
  return executeRequest(
      req,
      res,
      [res] (const char* data, size_t len) {
        res->appendBody(data, len);
      });
}

bool HTTPClient::executeRequest(
    const HTTPRequest& req,
    HTTPResponse* res,
    BodyChunkCallback on_body_chunk) {
  if (!conn_) {
    error_ = "not connected";
    return false;
  }

  if (!conn_->sendRequest(req)) {
    error_ = conn_->getError();
    return false;
  }

  if (!conn_->recvResponse(res)) {
    error_ = conn_->getError();
    return false;
  }

  return true;
}

std::string HTTPClient::getError() const {
  return error_;
}

} // namespace http
} // namespace libtransport


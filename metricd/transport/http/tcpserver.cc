/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "fnordmetric/util/exception.h"
#include "fnordmetric/util/inspect.h"
#include "fnordmetric/transport/http/tcpserver.h"

namespace fnordmetric {
namespace http {

TCPServer::TCPServer(
    TaskScheduler* scheduler) :
    scheduler_(scheduler),
    on_connection_cb_(nullptr) {}

void TCPServer::onConnection(
    std::function<void (std::unique_ptr<TCPConnection>)> callback) {
  on_connection_cb_ = callback;
}

void TCPServer::accept() {
  int conn_fd = ::accept(ssock_, NULL, NULL);

  if (conn_fd < 0) {
    RAISE_ERRNO(kIOError, "accept() failed");
  }

  if (on_connection_cb_) {
    scheduler_->run([this, conn_fd] () {
      on_connection_cb_(
          std::unique_ptr<TCPConnection>(new TCPConnection(conn_fd)));
    });
  }

  scheduler_->runOnReadable(std::bind(&TCPServer::accept, this), ssock_);
}

void TCPServer::listen(int port) {
  ssock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_ == 0) {
    RAISE_ERRNO(kIOError, "create socket() failed");
  }

  int opt = 1;
  if (setsockopt(ssock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    RAISE_ERRNO(kIOError, "setsockopt(SO_REUSEADDR) failed");
    return;
  }

  opt = 1;
  if (setsockopt(ssock_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    RAISE_ERRNO(kIOError, "setsockopt(SO_REUSEPORT) failed");
    return;
  }

  int flags = fcntl(ssock_, F_GETFL, 0);
  flags = flags | O_NONBLOCK;

  if (fcntl(ssock_, F_SETFL, flags) != 0) {
    RAISE_ERRNO(kIOError, "fnctl(%i) failed", ssock_);
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
  }

  scheduler_->runOnReadable(std::bind(&TCPServer::accept, this), ssock_);
}

}
}


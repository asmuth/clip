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

#include "metricd/util/exception.h"
#include "metricd/util/inspect.h"
#include "metricd/transport/http/tcpserver.h"

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


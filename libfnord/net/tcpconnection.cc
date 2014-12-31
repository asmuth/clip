/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "fnord/base/exception.h"
#include "fnord/base/inspect.h"
#include "fnord/net/tcpconnection.h"

namespace fnord {
namespace net {

TCPConnection::TCPConnection(int fd) : fd_(fd) {}

std::unique_ptr<TCPConnection> TCPConnection::connect(const InetAddr& addr) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    RAISE_ERRNO(kIOError, "socket() creation failed");
  }

  std::unique_ptr<TCPConnection> conn(new TCPConnection(fd));
  conn->connectImpl(addr);
  return conn;
}

void TCPConnection::connectImpl(const InetAddr& addr) {
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(addr.port());
  inet_aton(addr.ip().c_str(), &(saddr.sin_addr));
  memset(&(saddr.sin_zero), 0, 8);

  if (::connect(fd_, (const struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
    RAISE_ERRNO(kIOError, "connect() failed");
  }
}

int TCPConnection::fd() const {
  return fd_;
}

size_t TCPConnection::read(void* dst, size_t size) {
  auto res = ::read(fd_, dst, size);

  if (res < 0) {
    if (errno == EWOULDBLOCK) {
      RAISE(kWouldBlockError);
    } else {
      RAISE_ERRNO(kIOError, "read() failed");
    }
  }

  return res;
}

size_t TCPConnection::write(const void* data, size_t size) {
  auto res = ::write(fd_, data, size);

  if (res < 0) {
    RAISE_ERRNO(kIOError, "write() failed");
  }

  return res;
}

void TCPConnection::close() {
  ::close(fd_);
}

void TCPConnection::setNonblocking(bool nonblocking) {
  int flags = fcntl(fd_, F_GETFL, 0);

  if (nonblocking) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  if (fcntl(fd_, F_SETFL, flags) != 0) {
    RAISE_ERRNO(kIOError, "fnctl(%i) failed", fd_);
  }
}

void TCPConnection::checkErrors() const {

}

}
}


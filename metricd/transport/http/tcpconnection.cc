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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "metricd/util/exception.h"
#include "metricd/util/inspect.h"
#include "metricd/transport/http/tcpconnection.h"

namespace fnordmetric {
namespace http {

TCPConnection::TCPConnection(int fd) : fd_(fd), closed_(false) {}

TCPConnection::~TCPConnection() {
  if (!closed_) {
    close();
  }
}

std::unique_ptr<TCPConnection> TCPConnection::connect(const InetAddr& addr) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    RAISE_ERRNO(kIOError, "socket() creation failed");
  }

  std::unique_ptr<TCPConnection> conn(new TCPConnection(fd));
  conn->connectImpl(addr);
  return conn;
}

void TCPConnection::connectAsync(
    const InetAddr& addr,
    TaskScheduler* scheduler,
    std::function<void(std::unique_ptr<TCPConnection> conn)> on_ready) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    RAISE_ERRNO(kIOError, "socket() creation failed");
  }

  std::unique_ptr<TCPConnection> conn(new TCPConnection(fd));
  conn->setNonblocking();
  conn->connectImpl(addr);

  auto c = conn.get();
  scheduler->runOnWritable([on_ready, c] {
    on_ready(std::unique_ptr<TCPConnection>(c));
  }, *c);

  conn.release();
}

void TCPConnection::connectImpl(const InetAddr& addr) {
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(addr.port());
  inet_aton(addr.ip().c_str(), &(saddr.sin_addr));
  memset(&(saddr.sin_zero), 0, 8);

  if (::connect(fd_, (const struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
    if (errno != EINPROGRESS) {
      RAISE_ERRNO(kIOError, "connect() failed");
    }
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
  closed_ = true;
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

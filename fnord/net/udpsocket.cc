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
#include "fnord/net/udpsocket.h"

namespace fnord {
namespace net {

UDPSocket::UDPSocket() {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd_ == -1) {
    RAISE_ERRNO(kIOError, "socket() creation failed");
  }
}

UDPSocket::~UDPSocket() {
  close(fd_);
}

void UDPSocket::sendTo(const Buffer& pkt, const InetAddr& addr) {
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(addr.port());
  inet_aton(addr.ip().c_str(), &(saddr.sin_addr));
  memset(&(saddr.sin_zero), 0, 8);

  auto res = sendto(
      fd_,
      pkt.data(),
      pkt.size(),
      0,
      (struct sockaddr *) &saddr,
      sizeof(saddr));

  if (res < 0) {
    RAISE_ERRNO(kIOError, "sendTo() failed");
  }
}

}
}

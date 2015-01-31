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
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "fnord-base/exception.h"
#include "fnord-base/stringutil.h"
#include "fnord/net/inetaddr.h"

namespace fnord {
namespace net {

InetAddr InetAddr::resolve(const std::string& addr_str) {
  auto parts = StringUtil::split(addr_str, ":");
  unsigned port = 0;
  std::string hostname;
  std::string ip;

  switch (parts.size()) {
    case 2:
      port = std::stoi(parts[1]);
      /* fallthrough */

    case 1: {
      struct hostent* h = gethostbyname(parts[0].c_str());

      if (h == nullptr) {
        RAISEF(kResolveError, "gethostbyname($0) failed", parts[0]);
      }

      hostname = h->h_name;
      ip = inet_ntoa(*((struct in_addr *) h->h_addr));
      break;
    }

    default:
      RAISEF(kResolveError, "invalid address: $0", addr_str);
  }

  return InetAddr(hostname, ip, port);
}

InetAddr::InetAddr(
    const std::string& hostname,
    const std::string& ip,
    unsigned port) :
    hostname_(hostname),
    ip_(ip),
    port_(port) {}

const std::string& InetAddr::ip() const {
  return ip_;
}

const std::string& InetAddr::hostname() const {
  return hostname_;
}

std::string InetAddr::ipAndPort() const {
  return StringUtil::format("$0:$1", ip_, port_);
}

bool InetAddr::hasPort() const {
  return port_ > 0;
}

unsigned InetAddr::port() const {
  return port_;
}

void InetAddr::setPort(unsigned port) {
  port_ = port;
}

}
}

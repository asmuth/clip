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
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "metricd/util/exception.h"
#include "metricd/util/stringutil.h"
#include "metricd/transport/http/inetaddr.h"

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
      auto hstr = parts[0].c_str();

#ifdef HAVE_GETHOSTBYNAME_R
      size_t buflen = 1024;
      auto buf = static_cast<char*>(malloc(buflen));
      if (buf == nullptr) {
        RAISE(kMallocError, "malloc() failed");
      }

      struct hostent* h;
      struct hostent ret;
      int err;
      while (gethostbyname_r(hstr, &ret, buf, buflen, &h, &err) == ERANGE) {
        buflen *= 2;
        buf = static_cast<char*>(realloc(buf, buflen));
        if (buf == nullptr) {
          RAISE(kMallocError, "malloc() failed");
        }
      }

      if (h) {
        hostname = h->h_name;
        ip = inet_ntoa(*((struct in_addr *) h->h_addr));
        free(buf);
      } else {
        free(buf);
        RAISEF(kResolveError, "gethostbyname($0) failed", parts[0]);
      }
#else
      struct hostent* h = gethostbyname(hstr);

      if (h == nullptr) {
        RAISEF(kResolveError, "gethostbyname($0) failed", parts[0]);
      }

      hostname = h->h_name;
      ip = inet_ntoa(*((struct in_addr *) h->h_addr));
#endif
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

InetAddr::InetAddr(
    const std::string& hostname,
    unsigned port) :
    hostname_(hostname),
    port_(port) {}


const std::string& InetAddr::ip() const {
  if (ip_.length() == 0) {
    RAISE(kRuntimeError, "not resolved");
  }

  return ip_;
}

const std::string& InetAddr::hostname() const {
  return hostname_;
}

std::string InetAddr::ipAndPort() const {
  if (ip_.length() == 0) {
    RAISE(kRuntimeError, "not resolved");
  }

  return StringUtil::format("$0:$1", ip_, port_);
}

std::string InetAddr::hostAndPort() const {
  return StringUtil::format("$0:$1", hostname_, port_);
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


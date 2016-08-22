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
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "eventql/util/exception.h"
#include "eventql/util/stringutil.h"
#include "eventql/util/net/inetaddr.h"

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


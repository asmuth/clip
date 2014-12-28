/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/net/inetaddr.h"

namespace fnord {
namespace net {

InetAddr InetAddr::resolve(const std::string& addr_str) {
  return InetAddr("127.0.0.1", "127.0.0.1", 8080);
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

unsigned InetAddr::port() const {
  return port_;
}

}
}

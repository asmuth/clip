/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_INETADDR_H
#define _FNORD_INETADDR_H
#include <string>

namespace fnord {
namespace net {

class InetAddr {
public:

  static InetAddr resolve(const std::string& addr_str);

  const std::string& ip() const;
  const std::string& hostname() const;
  unsigned port() const;

protected:
  InetAddr(
      const std::string& hostname,
      const std::string& ip,
      unsigned port);

  std::string hostname_;
  std::string ip_;
  unsigned port_;
};

}
}

#endif

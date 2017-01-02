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
#ifndef _STX_INETADDR_H
#define _STX_INETADDR_H
#include <string>

class InetAddr {
public:

  static InetAddr resolve(const std::string& addr_str);

  InetAddr(
      const std::string& hostname,
      unsigned port);

  const std::string& ip() const;
  const std::string& hostname() const;
  unsigned port() const;
  bool hasPort() const;
  void setPort(unsigned port);
  std::string ipAndPort() const;
  std::string hostAndPort() const;

protected:

  InetAddr(
      const std::string& hostname,
      const std::string& ip,
      unsigned port);

  std::string hostname_;
  std::string ip_;
  unsigned port_;
};

#endif

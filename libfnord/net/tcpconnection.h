/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_NET_TCPCONNECTION_H
#define _FNORD_NET_TCPCONNECTION_H
#include <stdlib.h>

namespace fnord {
namespace net {

class TCPConnection {
public:

  TCPConnection(int fd);
  int fd() const;

  size_t read(void* dst, size_t size);
  size_t write(const void* data, size_t size);
  void close();
  void setNonblocking(bool nonblocking);

protected:
  int fd_;
};

}
}
#endif

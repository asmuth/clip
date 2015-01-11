/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_NET_UDPSOCKET_H
#define _FNORD_NET_UDPSOCKET_H
#include <stdlib.h>
#include "fnord/base/buffer.h"
#include "fnord/net/inetaddr.h"
#include "fnord/base/thread/taskscheduler.h"

namespace fnord {
namespace net {

class UDPSocket {
public:

  UDPSocket();
  ~UDPSocket();

  void sendTo(const Buffer& packet, const InetAddr& addr);

protected:
  int fd_;
};

}
}
#endif

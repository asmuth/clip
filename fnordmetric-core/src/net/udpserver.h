/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_NET_UDPSERVER_H
#define _FNORDMETRIC_NET_UDPSERVER_H
#include <fnordmetric/util/buffer.h>
#include <functional>

namespace fnord {
namespace net {

class UDPServer {
public:
  UDPServer();
  void onMessage(std::function<void (const util::Buffer&)> callback);
  void listen(int port);
protected:
  int ssock_;
  std::function<void (const fnord::util::Buffer&)> callback_;
};


}
}
#endif

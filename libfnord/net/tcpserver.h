/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_NET_TCPSERVER_H
#define _FNORD_NET_TCPSERVER_H

#include "fnord/thread/taskscheduler.h"
#include "fnord/net/tcpconnection.h"

namespace fnord {
namespace net {

class TCPServer {
public:

  TCPServer(
      thread::TaskScheduler* scheduler);

  void onConnection(
      std::function<void (std::unique_ptr<TCPConnection>)> callback);

  void listen(int port);

protected:
  void accept();

  int ssock_;
  thread::TaskScheduler* scheduler_;
  std::function<void (std::unique_ptr<TCPConnection>)> on_connection_cb_;
};

}
}
#endif

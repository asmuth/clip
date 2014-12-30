/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTP_CLIENTCONNECTION_H
#define _FNORD_HTTP_CLIENTCONNECTION_H
#include <memory>
#include <vector>
#include <fnord/logging/logger.h>
#include <fnord/net/http/httphandler.h>
#include <fnord/net/http/httpparser.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/net/inetaddr.h>
#include <fnord/net/tcpconnection.h>
#include <fnord/thread/taskscheduler.h>

namespace fnord {
namespace http {

class HTTPClientConnection {
public:
  static const size_t kMinBufferSize = 4096;

  HTTPClientConnection(
      std::unique_ptr<net::TCPConnection> conn,
      thread::TaskScheduler* scheduler);

  void executeRequest(const HTTPRequest& req);

protected:

  enum kHTTPClientConnectionState {
    S_CONN_INIT,
    S_CONN_CONNECTING,
    S_CONN_WRITE_REQ,
    S_CONN_READ_RES,
    S_CONN_IDLE,
    S_CONN_CLOSED,
    S_CONN_ERROR
  };

  //void read();
  //void write();
  //void awaitRead();
  //void awaitWrite();

  std::unique_ptr<net::TCPConnection> conn_;
  thread::TaskScheduler* scheduler_;
  kHTTPClientConnectionState state_;
  HTTPParser parser_;
  Buffer buf_;
  std::recursive_mutex mutex_;
};

}
}
#endif

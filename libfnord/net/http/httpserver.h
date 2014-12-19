/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_HTTPSERVER_H
#define _FNORDMETRIC_WEB_HTTPSERVER_H
#include <memory>
#include <vector>
#include <fnord/logging/logger.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httphandler.h>
#include <fnord/net/http/httpconnection.h>
#include <fnord/net/tcpserver.h>
#include <fnord/thread/taskscheduler.h>

namespace fnord {
namespace http {

using fnord::thread::TaskScheduler;

class HTTPServer {
public:
  HTTPServer(
      HTTPHandlerFactory* handler_factory,
      TaskScheduler* server_scheduler,
      TaskScheduler* request_scheduler);

  void listen(int port);

protected:
  void dispatchRequest(HTTPConnection* conn, HTTPRequest* req);

  HTTPHandlerFactory* handler_factory_;
  TaskScheduler* server_scheduler_;
  TaskScheduler* request_scheduler_;
  net::TCPServer ssock_;
};

}
}
#endif

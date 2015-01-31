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
#include <fnord-http/httprequest.h>
#include <fnord-http/httphandler.h>
#include "fnord-http/httpserverconnection.h"
#include <fnord-http/httpstats.h>
#include <fnord-base/net/tcpserver.h>
#include <fnord-base/thread/taskscheduler.h>

namespace fnord {
namespace http {

using fnord::TaskScheduler;

class HTTPServer {
public:
  HTTPServer(
      HTTPHandlerFactory* handler_factory,
      TaskScheduler* scheduler);

  void listen(int port);

  HTTPServerStats* stats();

protected:
  HTTPServerStats stats_;
  HTTPHandlerFactory* handler_factory_;
  TaskScheduler* scheduler_;
  net::TCPServer ssock_;
};

}
}
#endif

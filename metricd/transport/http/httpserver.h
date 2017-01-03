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
#ifndef _libstx_WEB_HTTPSERVER_H
#define _libstx_WEB_HTTPSERVER_H
#include <memory>
#include <vector>
#include <metricd/transport/http/httprequest.h>
#include <metricd/transport/http/httphandler.h>
#include "metricd/transport/http/httpserverconnection.h"
#include <metricd/transport/http/httpstats.h>
#include <metricd/transport/http/tcpserver.h>
#include <metricd/transport/http/taskscheduler.h>

namespace fnordmetric {
namespace http {

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
  TCPServer ssock_;
};

}
}
#endif

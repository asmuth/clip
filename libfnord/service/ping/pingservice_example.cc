/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include "fnord/base/application.h"
#include "fnord/base/inspect.h"
#include "fnord/net/http/httprouter.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/service/ping/pingservice.h"
#include "fnord/thread/eventloop.h"

using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::ping_service::PingService;

int main() {
  fnord::Application::init();
  fnord::Application::logToStderr();

  JSONRPC jsonrpc;
  JSONRPCHTTPAdapter rpc_http(&jsonrpc);

  PingService ping_service;
  jsonrpc.registerService<PingService>("PingService", &ping_service);

  fnord::http::HTTPRouter http_router;
  http_router.addRouteByPrefixMatch("/rpc", &rpc_http);

  fnord::thread::EventLoop event_loop;
  fnord::http::HTTPServer http_server(&http_router, &event_loop);
  http_server.listen(8080);
  event_loop.run();

  return 0;
}


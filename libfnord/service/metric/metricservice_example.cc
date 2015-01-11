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
#include "fnord/base/exceptionhandler.h"
#include "fnord/base/thread/eventloop.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/net/http/httprouter.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/service/metric/metricservice.h"
#include "fnord/service/metric/httpapiservlet.h"
#include "fnord/base/thread/threadpool.h"

using fnord::http::HTTPServer;
using fnord::http::HTTPRouter;
using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::metric_service::MetricService;
using fnord::thread::ThreadPool;

int main() {
  fnord::Application::init();
  fnord::Application::logToStderr();

  auto metric_service = MetricService::newWithInMemoryBackend();

  fnord::thread::EventLoop evloop;
  HTTPRouter http_router;
  HTTPServer http_server(&http_router, &evloop);
  http_server.listen(9999);

  fnord::metric_service::HTTPAPIServlet metrics_api(&metric_service);
  http_router.addRouteByPrefixMatch("/metrics", &metrics_api);

  JSONRPC jsonrpc;
  JSONRPCHTTPAdapter jsonrpc_http(&jsonrpc);
  http_router.addRouteByPrefixMatch("/rpc", &jsonrpc_http);

  evloop.run();
  return 0;
}


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
#include "fnord/base/exceptionhandler.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/service/metric/metricservice.h"
#include "fnord/service/metric/metricserviceadapter.h"
#include "fnord/base/thread/threadpool.h"
#include "fnord/system/signalhandler.h"

using fnord::http::HTTPServer;
using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::metric_service::MetricService;
using fnord::metric_service::MetricServiceAdapter;
using fnord::thread::ThreadPool;

int main() {
  fnord::CatchAndAbortExceptionHandler ehandler;
  ehandler.installGlobalHandlers();
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();

  JSONRPC rpc;

  auto metric_service = MetricService::newWithInMemoryBackend();
  MetricServiceAdapter::registerJSONRPC(&metric_service, &rpc);

  ThreadPool thread_pool;
  HTTPServer http_server(&thread_pool, &thread_pool);
  http_server.addHandler(JSONRPCHTTPAdapter::make(&rpc));
  http_server.listen(8080);

  return 0;
}


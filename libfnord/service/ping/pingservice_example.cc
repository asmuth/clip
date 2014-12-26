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
#include "fnord/base/inspect.h"
#include "fnord/comm/rpc.h"
#include "fnord/comm/rpcchannel.h"
#include "fnord/net/http/httprouter.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/logging/logoutputstream.h"
#include "fnord/reflect/reflect.h"
#include "fnord/service/ping/pingservice.h"
#include "fnord/system/signalhandler.h"
#include "fnord/thread/eventloop.h"
#include "fnord/thread/threadpool.h"

using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::ping_service::PingService;

int main() {
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();

  fnord::CatchAndAbortExceptionHandler ehandler;
  ehandler.installGlobalHandlers();

  fnord::log::LogOutputStream logger(fnord::io::OutputStream::getStderr());
  fnord::log::Logger::get()->setMinimumLogLevel(fnord::log::kDebug);
  fnord::log::Logger::get()->listen(&logger);

  fnord::thread::EventLoop event_loop;
  fnord::thread::ThreadPool thread_pool;

  JSONRPC jsonrpc;
  JSONRPCHTTPAdapter rpc_http(&jsonrpc);

  PingService ping_service;
  jsonrpc.registerService<PingService>("PingService", &ping_service);

  fnord::comm::LocalRPCChannel chan(&ping_service, &thread_pool);

  auto ping_rpc = fnord::comm::mkRPC(&PingService::ping, std::string("blah"));
  auto ping2_rpc = fnord::comm::mkRPC<std::string>("ping_async", std::string("fasdasd"), 123);

  ping_rpc.call(&chan);
  ping_rpc.wait();
  fnord::iputs("res: $0", ping_rpc.result());

  ping2_rpc.call(&chan);
  ping2_rpc.wait();
  fnord::iputs("res: $0", ping2_rpc.result());

  fnord::http::HTTPRouter http_router;
  http_router.addRouteByPrefixMatch("/rpc", &rpc_http);
  fnord::http::HTTPServer http_server(&http_router, &event_loop);
  http_server.listen(8080);
  event_loop.run();

  return 0;
}


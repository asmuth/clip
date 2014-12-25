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
#include "fnord/net/http/httprouter.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/reflect/reflect.h"
#include "fnord/service/ping/pingservice.h"
#include "fnord/service/ping/pingserviceadapter.h"
#include "fnord/system/signalhandler.h"
#include "fnord/thread/threadpool.h"

using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::ping_service::PingService;
using fnord::ping_service::PingServiceAdapter;

class ArgList {
public:
  template <typename T>
  T getArg(size_t index, const std::string& name) const {
    fnord::iputs("getarg: $0, $1", index, name);
    return 4444;
  };
};

int main() {
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();

  JSONRPC rpc;

  PingService ping_service;
  PingServiceAdapter::registerJSONRPC(&ping_service, &rpc);

  fnord::http::HTTPRouter http_router;
  fnord::thread::ThreadPool thread_pool;
  fnord::http::HTTPServer http_server(&http_router, &thread_pool);
  http_server.listen(8080);

  auto meta = fnord::reflect::reflect<fnord::ping_service::PingService>();
  auto method = dynamic_cast<const fnord::reflect::MethodCall<fnord::ping_service::PingService, std::string, int>*>(meta->method("ping2"));
  ArgList args;
  fnord::iputs("res: $0", method->call(&ping_service, args));

  return 0;
}


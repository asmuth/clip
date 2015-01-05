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
#include "fnord/net/http/httpserver.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/service/keyvalue/keyvalueservice.h"
#include "fnord/service/keyvalue/keyvalueserviceadapter.h"
#include "fnord/thread/threadpool.h"
#include "fnord/system/signalhandler.h"

using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::keyvalue_service::KeyValueService;
using fnord::keyvalue_service::KeyValueServiceAdapter;

int main() {
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();

  JSONRPC rpc;

  KeyValueService keyvalue_service;
  KeyValueServiceAdapter::registerJSONRPC(&keyvalue_service, &rpc);

  fnord::thread::ThreadPool thread_pool;
  fnord::http::HTTPServer http_server(&thread_pool, &thread_pool);
  http_server.addHandler(JSONRPCHTTPAdapter::make(&rpc));
  http_server.listen(8080);

  return 0;
}


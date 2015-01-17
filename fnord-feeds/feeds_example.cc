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
#include "fnord/net/http/httprouter.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/base/io/filerepository.h"
#include "fnord/base/io/fileutil.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "fnord/service/logstream/logstreamservice.h"
#include "fnord/base/thread/eventloop.h"
#include "fnord/base/thread/threadpool.h"

using fnord::json::JSONRPC;
using fnord::json::JSONRPCHTTPAdapter;
using fnord::logstream_service::LogStreamService;

int main() {
  fnord::Application::init();
  fnord::Application::logToStderr();

  JSONRPC rpc;
  JSONRPCHTTPAdapter rpc_http(&rpc);

  auto log_path = "/tmp/cm-logs";
  fnord::FileUtil::mkdir_p(log_path);

  LogStreamService ls_service{fnord::FileRepository(log_path)};
  rpc.registerService(&ls_service);

  fnord::thread::EventLoop event_loop;
  fnord::thread::ThreadPool thread_pool;
  fnord::http::HTTPRouter http_router;
  http_router.addRouteByPrefixMatch("/rpc", &rpc_http);
  fnord::http::HTTPServer http_server(&http_router, &event_loop);
  http_server.listen(8080);
  event_loop.run();

  event_loop.run();
  return 0;
}


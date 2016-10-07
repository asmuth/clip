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
#include "fnord/application.h"
#include "fnord/exceptionhandler.h"
#include "fnord/thread/eventloop.h"
#include "fnord/http/httpserver.h"
#include "fnord/http/httprouter.h"
#include "fnord/stats/statsd.h"
#include "fnord/json/jsonrpc.h"
#include "fnord/json/jsonrpchttpadapter.h"
#include "metricd/metricservice.h"
#include "metricd/httpapiservlet.h"
#include "fnord/thread/threadpool.h"

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

  fnord::statsd::StatsdServer statsd_server(&evloop, &evloop);
  statsd_server.onSample([&metric_service] (
      const std::string& key,
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels) {
    metric_service.insertSample(key, value, labels);
  });
  statsd_server.listen(8192);

  evloop.run();
  return 0;
}

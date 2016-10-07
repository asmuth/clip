/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <unistd.h>
#include "fnord/application.h"
#include "fnord/io/filerepository.h"
#include "fnord/io/fileutil.h"
#include "fnord/thread/eventloop.h"
#include "fnord/thread/threadpool.h"
#include "fnord/random.h"
#include "fnord/wallclock.h"
#include "fnord/cli/flagparser.h"
#include "fnord/http/httprouter.h"
#include "fnord/http/httpserver.h"
#include "fnord/http/httpconnectionpool.h"
#include "tsdb/TSDBClient.h"
#include "fnord/protobuf/msg.h"
#include <sensord/SensorSampleFeed.h>
#include <sensord/SensorPushServlet.h>
#include "metricd/metricservice.h"
#include "metricd/httpapiservlet.h"
#include "metricd/backends/tsdb/metricrepository.h"
#include "fnord/stats/statsd.h"

using namespace fnord;

int main(int argc, const char** argv) {
  Application::init();
  Application::logToStderr();

  cli::FlagParser flags;

  flags.defineFlag(
      "http",
      cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "8000",
      "Start the http server on this port",
      "<port>");

  flags.defineFlag(
      "tsdb",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      NULL,
      "tsdb addr",
      "<host:port>");

  flags.defineFlag(
      "loglevel",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  thread::EventLoop evloop;
  thread::ThreadPool tp(
      std::unique_ptr<ExceptionHandler>(
          new CatchAndLogExceptionHandler("metricd")));

  http::HTTPConnectionPool http(&evloop);
  tsdb::TSDBClient tsdb(
      StringUtil::format("http://$0/tsdb", flags.getString("tsdb")),
      &http);

  /* set up http server */
  http::HTTPRouter http_router;
  http::HTTPServer http_server(&http_router, &evloop);
  http_server.listen(flags.getInt("http"));
  http_server.stats()->exportStats("/metricd/http");

  sensord::SensorSampleFeed sensor_feed;
  //sensor_feed.subscribe(&tp, [&tsdb] (const sensord::SampleEnvelope& smpl) {
  //  if (smpl.sample_namespace().empty()) {
  //    fnord::logWarning("metricd", "discarding sample without namespace");
  //  }

  //  auto stream_key = StringUtil::format(
  //      "metricd.sensors.$0.$1",
  //      smpl.sample_namespace(),
  //      smpl.sensor_key());

  //  auto sample_data = smpl.data();
  //  auto sample_time = WallClock::unixMicros();

  //  tsdb.insertRecord(
  //      stream_key,
  //      sample_time,
  //      tsdb.mkMessageID(),
  //      Buffer(sample_data.data(), sample_data.size()));
  //});

  metricdb::SensorPushServlet sensor_servlet(&sensor_feed);
  http_router.addRouteByPrefixMatch("/sensors", &sensor_servlet);

  /* metric service */
  fnord::metric_service::MetricService metrics("metricd.metrics.", &tsdb);
  fnord::metric_service::HTTPAPIServlet metrics_api(&metrics);
  http_router.addRouteByPrefixMatch("/metrics", &metrics_api, &tp);

  fnord::statsd::StatsdServer statsd_server(&evloop, &tp);
  statsd_server.onSample([&metrics] (
      const std::string& key,
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels) {
    metrics.insertSample(key, value, labels);
  });

  statsd_server.listen(8192);
  //stats::StatsHTTPServlet stats_servlet;
  //http_router.addRouteByPrefixMatch("/stats", &stats_servlet);

  evloop.run();
  return 0;
}


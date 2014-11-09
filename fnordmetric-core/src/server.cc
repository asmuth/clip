/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/environment.h>
#include <fnordmetric/io/fileutil.h>
#include <fnordmetric/metricdb/adminui.h>
#include <fnordmetric/metricdb/httpapi.h>
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/metricdb/backends/disk/metricrepository.h>
#include <fnordmetric/metricdb/backends/inmemory/metricrepository.h>
#include <fnordmetric/metricdb/statsd.h>
#include <fnordmetric/net/udpserver.h>
#include <fnordmetric/util/exceptionhandler.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/random.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/signalhandler.h>
#include <fnordmetric/thread/threadpool.h>
#include <fnordmetric/thread/task.h>
#include <xzero/TimeSpan.h>
#include <xzero/http/HttpService.h>
#include <xzero/executor/ThreadedExecutor.h>
#include <xzero/net/IPAddress.h>
#include <xzero/net/InetConnector.h>
#include <xzero/support/libev/LibevScheduler.h>
#include <xzero/support/libev/LibevSelector.h>
#include <xzero/support/libev/LibevClock.h>
#include <ev++.h>

using namespace fnordmetric;
using namespace fnordmetric::metricdb;

static const char kCrashErrorMsg[] =
    "FnordMetric crashed :( -- Please report a bug at "
    "github.com/paulasmuth/fnordmetric";

using fnord::thread::Task;
using fnord::thread::TaskScheduler;

IMetricRepository* openBackend(
    const std::string& backend_type,
    TaskScheduler* backend_scheduler) {
  /* open inmemory backend */
  if (backend_type == "inmemory") {
    env()->logger()->printf(
        "INFO",
        "Opening new inmemory backend -- SHOULD ONlY BE USED FOR TESTING");
    return new inmemory_backend::MetricRepository();
  }

  /* open disk backend */
  if (backend_type == "disk") {
    if (!env()->flags()->isSet("datadir")) {
      RAISE(
          kIllegalArgumentError,
          "the --datadir flag must be set when using the disk backend");
    }

    auto datadir = env()->flags()->getString("datadir");
    env()->logger()->printf(
        "INFO",
        "Opening disk backend at %s",
        datadir.c_str());

    return new disk_backend::MetricRepository(datadir, backend_scheduler);
  }

  RAISE(
      kIllegalArgumentError,
      "unknown backend type: %s",
      backend_type.c_str());
}

int main(int argc, const char** argv) {
  fnord::util::CatchAndAbortExceptionHandler ehandler(kCrashErrorMsg);
  ehandler.installGlobalHandlers();

  fnordmetric::util::SignalHandler::ignoreSIGHUP();
  fnordmetric::util::SignalHandler::ignoreSIGPIPE();

  fnord::util::Random::init();

  env()->flags()->defineFlag(
      "storage_backend",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      "disk",
      "One of 'disk', 'inmemory', 'mysql' or 'hbase'. Default: 'disk'",
      "<name>");

  // flags
  env()->flags()->defineFlag(
      "datadir",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      "/tmp/fnordmetric-data",
      "Store the database in this directory (disk backend only)",
      "<path>");

  env()->flags()->defineFlag(
      "http_port",
      cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "8080",
      "Start the web interface on this port",
      "<port>");

  env()->flags()->defineFlag(
      "statsd_port",
      cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "8125",
      "Start the statsd interface on this port",
      "<port>");

  env()->flags()->parseArgv(argc, argv);
  env()->setVerbose(true);

  // boot
  fnord::thread::ThreadPool thread_pool(
      std::unique_ptr<fnord::util::ExceptionHandler>(
          new fnord::util::CatchAndAbortExceptionHandler(kCrashErrorMsg)));

  auto datadir = env()->flags()->getString("datadir");
  if (!fnord::io::FileUtil::exists(datadir)) {
    env()->logger()->printf(
        "FATAL",
        "File %s does not exist",
        datadir.c_str());

    return 1;
  }

  if (!fnord::io::FileUtil::isDirectory(datadir)) {
    env()->logger()->printf(
        "FATAL",
        "FIle %s is not a directory",
        datadir.c_str());

    return 1;
  }


  auto metric_repo = openBackend(
      env()->flags()->getString("storage_backend"),
      &thread_pool);

  /* statsd server */
  if (env()->flags()->isSet("statsd_port")) {
    auto port = env()->flags()->getInt("statsd_port");
    auto statsd_server =
        new StatsdServer(metric_repo, &thread_pool, &thread_pool);
    statsd_server->listen(port);

    env()->logger()->printf(
        "INFO",
        "Starting statsd server on port %i",
        port);
  }

  /* http server */
  if (env()->flags()->isSet("http_port")) {
    auto port = env()->flags()->getInt("http_port");
    xzero::IPAddress bind("0.0.0.0");
    xzero::TimeSpan idle = xzero::TimeSpan::fromSeconds(30);
    xzero::HttpService http;
    HTTPAPI fmHttpApi(metric_repo);
    http.addHandler(&fmHttpApi);
    http.addHandler(fnordmetric::metricdb::AdminUI::get());

    // single-threaded non-blocking execution
    ::ev::loop_ref loop = ::ev::default_loop(0);
    xzero::support::LibevScheduler scheduler(loop);
    xzero::support::LibevSelector selector(loop);
    xzero::support::LibevClock clock(loop);

    auto inet = http.configureInet(&scheduler, &scheduler, &selector, &clock,
                                   idle, bind, port);
    inet->setBlocking(false);

    env()->logger()->printf(
        "INFO",
        "Starting HTTP server on port %i",
        port);

    http.start();
    selector.select();
  }

  return 0;
}


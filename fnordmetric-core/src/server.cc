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
#include <fnordmetric/metricdb/compactiontask.h>
#include <fnordmetric/metricdb/httpapi.h>
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/net/udpserver.h>
#include <fnordmetric/util/exceptionhandler.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/random.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/signalhandler.h>
#include <thread>
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

int main(int argc, const char** argv) {
  fnordmetric::util::CatchAndAbortExceptionHandler ehandler(kCrashErrorMsg);
  ehandler.installGlobalHandlers();

  fnordmetric::util::SignalHandler::ignoreSIGHUP();
  fnordmetric::util::SignalHandler::ignoreSIGPIPE();

  fnord::util::Random::init();

  // flags
  env()->flags()->defineFlag(
      "datadir",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      NULL,
      "Store the database in this directory",
      "<path>");

  env()->flags()->defineFlag(
      "port",
      cli::FlagParser::T_INTEGER,
      false,
      NULL,
      NULL,
      "Start the web interface on this port",
      "<port>");

  env()->flags()->parseArgv(argc, argv);
  env()->setVerbose(true);

  // boot
  fnordmetric::util::ThreadPool thread_pool(
      32,
      std::unique_ptr<fnordmetric::util::ExceptionHandler>(
          new fnordmetric::util::CatchAndPrintExceptionHandler(
              env()->logger())));

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

  env()->logger()->printf("INFO", "Opening database at %s", datadir.c_str());
  std::shared_ptr<fnord::io::FileRepository> file_repo(
      new fnord::io::FileRepository(datadir));
  MetricRepository metric_repo(file_repo);

  CompactionTask compaction_task(&metric_repo);
  thread_pool.run(compaction_task.runnable());

  thread_pool.run([] () {
    fnord::net::UDPServer statsd_server;
    statsd_server.onMessage([] (const fnord::util::Buffer& msg) {
      printf("msg: %s\n", msg.toString().c_str());
    });
    statsd_server.listen(1337);
  });

  auto port = env()->flags()->getInt("port");
  xzero::IPAddress bind("0.0.0.0");
  xzero::TimeSpan idle = xzero::TimeSpan::fromSeconds(30);
  xzero::HttpService http;
  HTTPAPI fmHttpApi(&metric_repo);
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
      "Starting HTTP server on port %i (single threaded non-blocking)",
      port);

  http.start();
  selector.select();

  return 0;
}


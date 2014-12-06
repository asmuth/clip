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
#include <fnord/base/exception.h>
#include <fnord/base/exceptionhandler.h>
#include <fnord/base/random.h>
#include <fnord/net/http/httpserver.h>
#include <fnord/io/fileutil.h>
#include <fnord/io/inputstream.h>
#include <fnord/io/outputstream.h>
#include <fnord/net/udpserver.h>
#include <fnord/net/statsd/statsd.h>
#include <fnord/thread/threadpool.h>
#include <fnord/service/metric/metricservice.h>
#include <fnord/system/signalhandler.h>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/environment.h>
#include <fnordmetric/adminui.h>
#include <fnordmetric/httpapi.h>

using namespace fnordmetric;

static const char kCrashErrorMsg[] =
    "FnordMetric crashed :( -- Please report a bug at "
    "github.com/paulasmuth/fnordmetric";

using fnord::metric_service::MetricService;
using fnord::thread::Task;
using fnord::thread::TaskScheduler;
using fnord::statsd::StatsdServer;

static MetricService makeMetricService(
    const std::string& backend_type,
    TaskScheduler* backend_scheduler) {

  /* open inmemory backend */
  if (backend_type == "inmemory") {
    env()->logger()->printf(
        "INFO",
        "Opening new inmemory backend -- SHOULD ONlY BE USED FOR TESTING");
    return MetricService::newWithInMemoryBackend();
  }

  /* open disk backend */
  if (backend_type == "disk") {
    if (!env()->flags()->isSet("datadir")) {
      RAISE(
          kUsageError,
          "the --datadir flag must be set when using the disk backend");
    }

    auto datadir = env()->flags()->getString("datadir");
    env()->logger()->printf(
        "INFO",
        "Opening disk backend at %s",
        datadir.c_str());

    return MetricService::newWithDiskBackend(datadir, backend_scheduler);
  }

  RAISE(
      kUsageError,
      "unknown backend type: %s",
      backend_type.c_str());
}

static int startServer() {
  fnord::thread::ThreadPool server_pool(
      std::unique_ptr<fnord::ExceptionHandler>(
          new fnord::CatchAndAbortExceptionHandler(kCrashErrorMsg)));

  fnord::thread::ThreadPool worker_pool(
      std::unique_ptr<fnord::ExceptionHandler>(
          new fnord::CatchAndPrintExceptionHandler(nullptr)));

  if (env()->flags()->isSet("datadir")) {
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
          "File %s is not a directory",
          datadir.c_str());

      return 1;
    }
  }

  auto metric_service = makeMetricService(
      env()->flags()->getString("storage_backend"),
      &server_pool);

  /* statsd server */
  if (env()->flags()->isSet("statsd_port")) {
    auto port = env()->flags()->getInt("statsd_port");
    env()->logger()->printf(
        "INFO",
        "Starting statsd server on port %i",
        port);

    auto statsd_server = new StatsdServer(&server_pool, &worker_pool);
    statsd_server->listen(port);
  }

  /* http server */
  if (env()->flags()->isSet("http_port")) {
    auto port = env()->flags()->getInt("http_port");
    env()->logger()->printf(
        "INFO",
        "Starting HTTP server on port %i",
        port);

    auto http_server = new fnord::http::HTTPServer(
        &server_pool,
        &worker_pool);

    http_server->addHandler(AdminUI::getHandler());
    //http_server->addHandler(
    //    std::unique_ptr<http::HTTPHandler>(new HTTPAPI(metric_repo)));
    http_server->listen(port);
  }

  return 0;
}

static void printUsage() {
  auto err_stream = fnord::io::OutputStream::getStderr();
  err_stream->printf("usage: fnordmetric-server [options]\n");
  err_stream->printf("\noptions:\n");
  env()->flags()->printUsage(err_stream.get());
  err_stream->printf("\nexamples:\n");
  err_stream->printf("    $ fnordmetric-server --http_port 8080 --statsd_port 8125 --datadir /tmp/fnordmetric-data\n");
}

int main(int argc, const char** argv) {
  fnord::CatchAndAbortExceptionHandler ehandler(kCrashErrorMsg);
  ehandler.installGlobalHandlers();
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();
  fnord::Random::init();

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

  env()->flags()->defineFlag(
      "storage_backend",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      "disk",
      "One of 'disk', 'inmemory', 'mysql' or 'hbase'. Default: 'disk'",
      "<name>");

  env()->flags()->defineFlag(
      "datadir",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      NULL,
      "Store the database in this directory (disk backend only)",
      "<path>");

  env()->flags()->defineFlag(
      "disable_external_sources",
      cli::FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Disable queries against external data sources like CSV files or MySQL");

  env()->flags()->defineFlag(
      "verbose",
      cli::FlagParser::T_SWITCH,
      false,
      NULL,
      "Be verbose");

  env()->flags()->defineFlag(
      "help",
      cli::FlagParser::T_SWITCH,
      false,
      "h",
      NULL,
      "You are reading it...");

  env()->flags()->parseArgv(argc, argv);
  env()->setVerbose(env()->flags()->isSet("verbose"));

  if (env()->flags()->isSet("help")) {
    printUsage();
    return 0;
  }

  try {
    return startServer();
  } catch (const fnord::Exception& e) {
    auto err_stream = fnord::io::OutputStream::getStderr();
    auto msg = e.getMessage();
    err_stream->printf("[ERROR] ");
    err_stream->write(msg.c_str(), msg.size());
    err_stream->printf("\n");

    if (e.getTypeName() == kUsageError) {
      err_stream->printf("\n");
      printUsage();
    } else {
      env()->logger()->exception("FATAL", "Fatal error", e);
    }

    return 1;
  }

  return 0;
}


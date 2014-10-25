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
#include <fnordmetric/ev/acceptor.h>
#include <fnordmetric/ev/eventloop.h>
#include <fnordmetric/http/httpserver.h>
#include <fnordmetric/metricdb/adminui.h>
#include <fnordmetric/util/exceptionhandler.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/signalhandler.h>
#include <fnordmetric/thread/threadpool.h>

using namespace fnordmetric;
using namespace fnordmetric::metricdb;

static const char kCrashErrorMsg[] =
    "FnordMetric crashed :( -- Please report a bug at "
    "github.com/paulasmuth/fnordmetric";

int main(int argc, const char** argv) {
  util::CatchAndAbortExceptionHandler ehandler(kCrashErrorMsg);
  ehandler.installGlobalHandlers();

  util::SignalHandler::ignoreSIGHUP();
  util::SignalHandler::ignoreSIGPIPE();

  // flags
  env()->flags()->defineFlag(
      "port",
      cli::FlagParser::T_INTEGER,
      false,
      NULL,
      NULL,
      "Start the web interface on this port",
      "<port>");

  env()->flags()->parseArgv(argc, argv);

  // boot
  util::ThreadPool thread_pool(
      32,
      std::unique_ptr<util::ExceptionHandler>(
          new util::CatchAndPrintExceptionHandler(env()->logger())));

  ev::EventLoop ev_loop;
  ev::Acceptor acceptor(&ev_loop);
  http::ThreadedHTTPServer http(&thread_pool);
  http.addHandler(AdminUI::getHandler());

  auto port = env()->flags()->getInt("port");
  env()->logger()->printf("INFO", "Starting HTTP server on port %i", port);
  acceptor.listen(port, &http);
  ev_loop.loop();

  return 0;
}


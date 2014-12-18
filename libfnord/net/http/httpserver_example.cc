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
#include <unistd.h>
#include "fnord/base/exception.h"
#include "fnord/base/exceptionhandler.h"
#include "fnord/net/http/httpserver.h"
#include "fnord/logging/logger.h"
#include "fnord/logging/logoutputstream.h"
#include "fnord/thread/threadpool.h"
#include "fnord/system/signalhandler.h"

int main() {
  fnord::system::SignalHandler::ignoreSIGHUP();
  fnord::system::SignalHandler::ignoreSIGPIPE();

  fnord::CatchAndAbortExceptionHandler ehandler;
  ehandler.installGlobalHandlers();

  fnord::log::LogOutputStream logger(fnord::io::OutputStream::getStderr());
  fnord::log::Logger::get()->setMinimumLogLevel(fnord::log::kTrace);
  fnord::log::Logger::get()->listen(&logger);

  fnord::thread::ThreadPool thread_pool;
  fnord::http::HTTPServer http_server(&thread_pool, &thread_pool);
  http_server.listen(8080);

  for (;;) { usleep(100000); }

  return 0;
}


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/application.h"
#include "fnord-base/logging.h"
#include "fnord-base/logging/logoutputstream.h"
#include "fnord-base/exceptionhandler.h"
#include "fnord-base/thread/signalhandler.h"

namespace fnord {

void Application::init() {
  fnord::thread::SignalHandler::ignoreSIGHUP();
  fnord::thread::SignalHandler::ignoreSIGPIPE();

  auto ehandler = new fnord::CatchAndAbortExceptionHandler();
  ehandler->installGlobalHandlers();
}

void Application::logToStderr(LogLevel min_log_level /* = LogLevel::kInfo */) {
  auto logger = new LogOutputStream(OutputStream::getStderr());
  Logger::get()->setMinimumLogLevel(min_log_level);
  Logger::get()->addTarget(logger);
}

}

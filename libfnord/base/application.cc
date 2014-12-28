/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/application.h"
#include "fnord/base/exceptionhandler.h"
#include "fnord/thread/signalhandler.h"
#include "fnord/logging/logoutputstream.h"

namespace fnord {

void Application::init() {
  fnord::thread::SignalHandler::ignoreSIGHUP();
  fnord::thread::SignalHandler::ignoreSIGPIPE();

  auto ehandler = new fnord::CatchAndAbortExceptionHandler();
  ehandler->installGlobalHandlers();
}

void Application::logToStderr() {
  auto logger = new fnord::log::LogOutputStream(
      fnord::io::OutputStream::getStderr());

  fnord::log::Logger::get()->setMinimumLogLevel(fnord::log::kDebug);
  fnord::log::Logger::get()->listen(logger);
}

}

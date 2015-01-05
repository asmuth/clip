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
#include <fnord/base/exception.h>
#include <fnord/base/exceptionhandler.h>
#include <fnord/base/inspect.h>

namespace fnord {

using fnord::Exception;

CatchAndPrintExceptionHandler::CatchAndPrintExceptionHandler(
    Logger* logger) :
    logger_(logger) {}

void CatchAndPrintExceptionHandler::onException(
    const std::exception& error) const {
  fnord::iputs("ERROR: Uncaught exception $0", error);
  //logger_->exception("ERROR", "Uncaught exception", error);
}

CatchAndAbortExceptionHandler::CatchAndAbortExceptionHandler(
    const std::string& message) :
    message_(message) {}

void CatchAndAbortExceptionHandler::onException(
    const std::exception& error) const {
  fprintf(stderr, "%s\n\n", message_.c_str()); // FIXPAUL

  try {
    auto rte = dynamic_cast<const fnord::Exception&>(error);
    rte.debugPrint();
  } catch (const std::exception& cast_error) {
    fprintf(stderr, "foreign exception: %s\n", error.what());
  }

  fprintf(stderr, "Aborting...\n");
  abort(); // core dump if enabled
}

static std::string globalEHandlerMessage;
static void globalEHandler() {
  fprintf(stderr, "%s\n", globalEHandlerMessage.c_str());

  try {
    throw;
  } catch (const std::exception& e) {
    try {
      auto rte = dynamic_cast<const fnord::Exception&>(e);
      rte.debugPrint();
      exit(1);
    } catch (...) {
      fprintf(stderr, "foreign exception: %s\n", e.what());
      /* fallthrough */
    }
  } catch (...) {
    /* fallthrough */
  }

  abort();
}

void CatchAndAbortExceptionHandler::installGlobalHandlers() {
  globalEHandlerMessage = message_;
  std::set_terminate(&globalEHandler);
  std::set_unexpected(&globalEHandler);
}

} // namespace fnord


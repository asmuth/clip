/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/exceptionhandler.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace util {

CatchAndPrintExceptionHandler::CatchAndPrintExceptionHandler(
    Logger* logger) :
    logger_(logger) {}

void CatchAndPrintExceptionHandler::onException(
    const std::exception& error) const {
  logger_->exception("ERROR", "Uncaught exception", error);
}

CatchAndAbortExceptionHandler::CatchAndAbortExceptionHandler(
    const std::string& message) :
    message_(message) {}

void CatchAndAbortExceptionHandler::onException(
    const std::exception& error) const {
  fprintf(stderr, "%s", message_.c_str()); // FIXPAUL

  try {
    auto rte = dynamic_cast<const util::RuntimeException&>(error);
    rte.debugPrint();
    exit(1);
  } catch (const std::exception& e) {
    fprintf(stderr, "Aborting...\n");
    abort(); // core dump if enabled
  }
}

}
}

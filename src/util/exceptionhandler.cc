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
    std::shared_ptr<OutputStream> os) :
    os_(std::move(os)) {}

CatchAndPrintExceptionHandler::CatchAndPrintExceptionHandler() :
    CatchAndPrintExceptionHandler(OutputStream::getStderr()) {}

void CatchAndPrintExceptionHandler::onException(std::exception* error) const {
  os_->mutex_.lock();
  try {
    os_->printf("[ERROR] Uncaught exception!\n");
    RuntimeException* rte = dynamic_cast<RuntimeException *>(error);
    if (rte != nullptr) {
      rte->debugPrint(os_.get());
    }
  } catch (std::exception e) {
    /* oopsiedaisy */
  }

  os_->mutex_.unlock();
}

}
}

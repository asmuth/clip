/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_EXCEPTIONHANDLER_H
#define _FNORDMETRIC_UTIL_EXCEPTIONHANDLER_H
#include <mutex>
#include <memory>
#include <string>

namespace fnord {
class Logger;

class ExceptionHandler {
public:
  virtual ~ExceptionHandler() {}
  virtual void onException(const std::exception& error) const = 0;
};

class CatchAndPrintExceptionHandler : public ExceptionHandler {
public:
  CatchAndPrintExceptionHandler(Logger* logger);
  void onException(const std::exception& error) const override;
protected:
  Logger* logger_;
};

class CatchAndAbortExceptionHandler : public ExceptionHandler {
public:
  CatchAndAbortExceptionHandler(const std::string& message = "Aborting...");
  void onException(const std::exception& error) const override;

  void installGlobalHandlers();

protected:
  std::string message_;
};

}
#endif

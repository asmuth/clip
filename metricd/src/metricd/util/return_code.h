/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include "stringutil.h"

class ReturnCode {
public:

  inline static ReturnCode success() {
    return ReturnCode(true);
  }

  inline static ReturnCode error(
      const std::string& code,
      const std::string& message) {
    ReturnCode rc(false);
    rc.code_ = code;
    rc.message_ = message;
    return rc;
  }

  inline static ReturnCode errorp(
      const std::string& code,
      const char* message,
      ...) {
    ReturnCode rc(false);
    rc.code_ = code;

    va_list args;
    va_start(args, message);
    int bufsize = 1024;//vsnprintf(nullptr, 0, message, args);
    rc.message_.resize(bufsize + 1);
    bufsize = vsnprintf(&rc.message_[0], rc.message_.size(), message, args);
    rc.message_.resize(bufsize);
    va_end(args);

    return rc;
  }

  template <typename... T>
  inline static ReturnCode errorf(
      const std::string& code,
      const std::string& message,
      T... args) {
    return ReturnCode::error(code, StringUtil::format(message, args...));
  }

  inline bool isError() const {
    return !success_;
  }

  inline bool isSuccess() const {
    return success_;
  }

  inline const std::string& getCode() const {
    return code_;
  }

  inline const std::string& getMessage() const {
    return message_;
  }

protected:
  ReturnCode(bool success) : success_(success) {}
  bool success_;
  std::string code_;
  std::string message_;
};


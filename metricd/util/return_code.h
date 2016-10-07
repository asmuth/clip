/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#pragma once
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <stdarg.h>

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

  inline static ReturnCode error(
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


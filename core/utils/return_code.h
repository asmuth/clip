/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include <initializer_list>
#include "stringutil.h"
#include "fviz.h"

namespace fviz {

enum Status : int {
  ERROR = 0,
  OK = 1,
};

class ReturnCode {
public:

  inline static ReturnCode success() {
    return ReturnCode(true);
  }

  inline static ReturnCode error(
      const std::string& code,
      const std::string& message) {
    ReturnCode rc(false);
    rc.message_ = message;
    return rc;
  }

  inline static ReturnCode errorp(
      const std::string& code,
      const char* message,
      ...) {
    ReturnCode rc(false);
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

  inline ReturnCode(Status status) :
      success_(status == OK),
      message_("runtime error") {}

  inline ReturnCode(Status status, std::string message) :
      success_(status != ERROR),
      message_(message) {}

  inline bool isError() const {
    return !success_;
  }

  inline bool isSuccess() const {
    return success_;
  }

  inline operator bool() const {
    return success_;
  }

  inline const std::string& getMessage() const {
    return message_;
  }

protected:
  ReturnCode(bool success) : success_(success) {}
  bool success_;
  std::string message_;
};

inline ReturnCode try_chain(
    const std::initializer_list<std::function<ReturnCode ()>>& op) {
  for (const auto& o : op) {
    if (auto rc = o(); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode err_invalid_value(
    const std::string& value,
    std::vector<std::string> expected_values);

} // namespace fviz


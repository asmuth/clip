/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


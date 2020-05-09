/**
 * This file is part of the "clip" project
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
#include <fmt/format.h>
#include "stringutil.h"

namespace clip {
struct Expr;

enum Status : int {
  ERROR = 0,
  OK = 1,
};

struct ReturnCode {
  ReturnCode() : code(OK) {}
  ReturnCode(Status c) : code(c) {}
  ReturnCode(Status c, const std::string& m) : code(c), message(m) {}

  operator bool() const { return code == OK; }

  Status code;
  std::string message;
  std::list<const Expr*> trace;
};

inline ReturnCode error(
    const Status code,
    const std::string& message) {
  return ReturnCode(code, message);
}

template <typename... T>
inline static ReturnCode errorf(
    const Status code,
    const std::string& message,
    T... args) {
  return ReturnCode(code, fmt::format(message, args...));
}

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

ReturnCode err_invalid_nargs(size_t nargs, size_t nargs_expect);

void error_print(const ReturnCode& rc, std::ostream& os);

} // namespace clip


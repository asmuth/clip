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
#include "return_code.h"
#include "sexpr.h"

#include <iostream>

namespace fviz {

ReturnCode err_invalid_value(
    const std::string& value,
    std::vector<std::string> expected_values) {
  for (auto& v : expected_values) {
    v = "'" + v + "'";
  }

  return {
    ERROR,
    fmt::format(
        "invalid value: '{}'; expected one of: {}",
        value,
        StringUtil::join(expected_values, ", "))
  };
}

void error_print(const ReturnCode& rc, std::ostream& os) {
  switch (rc.code) {
    case OK:
      os << "OK" << std::endl;
      break;
    case ERROR:
      os << "ERROR" << std::endl;
      break;
  }

  if (rc) {
    return;
  }

  os
    << "  | " << rc.message
    << std::endl
    << std::endl
    << "TRACEBACK"
    << std::endl;

  for (const auto& t : rc.trace) {
    os << "  |> " << expr_inspect(t) << std::endl;
  }
}

} // namespace fviz


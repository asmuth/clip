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

namespace fviz {

ReturnCode err_invalid_value(
    const std::string& value,
    std::vector<std::string> expected_values) {
  for (auto& v : expected_values) {
    v = "'" + v + "'";
  }

  return {
    ERROR,
    StringUtil::format(
        "invalid value: '$0'; expected one of: $1",
        value,
        StringUtil::join(expected_values, ", "))
  };
}

} // namespace fviz


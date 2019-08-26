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

namespace clip {

template <typename T>
ReturnCode data_load_as(
    const Expr* expr,
    std::function<ReturnCode (const std::string&, T*)> conv,
    std::vector<T>* dst) {
  std::vector<std::string> data;
  if (auto rc = data_load_strings(expr, &data); !rc) {
    return rc;
  }

  for (const auto& d : data) {
    T v;
    if (auto rc = conv(d, &v); !rc) {
      return rc;
    }

    dst->emplace_back(std::move(v));
  }

  return OK;
}

} // namespace clip


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

namespace fviz {

/*
template<typename T>
ReturnCode parseEnum(
    const EnumDefinitions<T>& defs,
    const std::string& str,
    T* value) {
  const auto& def = defs.find(str);
  if (def == defs.end()) {
    return ReturnCode::errorf("EPARSE", "invalid value '$0'", str);
  }

  *value = def->second;
  return ReturnCode::success();
}

template <typename T>
ParserFn configure_vec(ParseToFn<T> parser, std::vector<T>* values) {
  return [parser, values] (const plist::Property& prop) -> ReturnCode {
    switch (plist::is_list(prop)) {

      case true: {
        for (const auto& cld : *prop.next) {
          T val;
          if (auto rc = parser(cld, &val); !rc) {
            return rc;
          }

          values->emplace_back(val);
        }

        return OK;
      }

      case false: {
        T val;
        if (auto rc = parser(prop, &val); !rc) {
          return rc;
        }

        values->emplace_back(val);
        return OK;
      }

    }
  };
}
*/

} // namespace fviz


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <unordered_map>
#include "utils/return_code.h"

namespace signaltk {

template <typename T>
using PropertyDefinitions = std::unordered_map<
    std::string,
    std::function<ReturnCode (const std::string&, T*)>>;

template<typename T>
ReturnCode configureProperties(
    const PropertyList& plist,
    const PropertyDefinitions<T>& pdefs,
    T* config) {
  for (const auto& prop : plist.properties) {
    const auto& pdef = pdefs.find(prop.first);
    if (pdef != pdefs.end()) {
      if (auto rc = pdef->second(prop.second, config); !rc.isSuccess()) {
        return rc;
      }
    }
  }

  return ReturnCode::success();
}

template <typename T>
using EnumDefinitions = std::unordered_map<std::string, T>;

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

} // namespace signaltk


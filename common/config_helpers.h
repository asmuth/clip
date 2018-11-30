/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include <unordered_map>
#include <plist/plist.h>
#include "utils/return_code.h"

namespace plotfx {

using ParserDefinitions = std::unordered_map<
    std::string,
    std::function<ReturnCode (const plist::Property&)>>;

inline ReturnCode parseAll(
    const plist::PropertyList& plist,
    const ParserDefinitions& pdefs) {
  for (const auto& prop : plist) {
    const auto& pdef = pdefs.find(prop.name);
    if (pdef != pdefs.end()) {
      if (auto rc = pdef->second(prop); !rc.isSuccess()) {
        return ReturnCode::errorf(
            "EPARSE",
            "error while parsing property '$0': $1",
            prop.name,
            rc.getMessage());

        return rc;
      }
    }
  }

  return ReturnCode::success();
}

template <typename T>
using PropertyDefinitions = std::unordered_map<
    std::string,
    std::function<ReturnCode (const plist::Property&, T*)>>;

template<typename T>
ReturnCode configureProperties(
    const plist::PropertyList& plist,
    const PropertyDefinitions<T>& pdefs,
    T* config) {
  for (const auto& prop : plist) {
    const auto& pdef = pdefs.find(prop.name);
    if (pdef != pdefs.end()) {
      if (auto rc = pdef->second(prop, config); !rc.isSuccess()) {
        return ReturnCode::errorf(
            "EPARSE",
            "error while parsing property '$0': $1",
            prop.name,
            rc.getMessage());

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

ReturnCode parseDataSeries(
    const plist::Property& prop,
    std::vector<double>* data);

} // namespace plotfx


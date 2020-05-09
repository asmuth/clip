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
#include <string>
#include <vector>
#include "return_code.h"

namespace clip {

struct FlagInfo {
  char opt_short;
  std::string opt_long;
  bool has_arg;
  std::function<void (const char*)> callback;
};

using FlagList = std::vector<FlagInfo>;

void flags_add_string(
      FlagList* flags,
      char shortopt,
      const std::string& longopt,
      std::string* value);

void flags_add_stringv(
      FlagList* flags,
      char shortopt,
      const std::string& longopt,
      std::vector<std::string>* values);

void flags_add_switch(
      FlagList* flags,
      char shortopt,
      const std::string& longopt,
      bool* value);

ReturnCode flags_parse(
    const FlagList& flags,
    int argc,
    char** argv,
    std::vector<std::string>* args_out);

}

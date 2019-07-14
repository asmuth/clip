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
#include <string>
#include <vector>
#include "return_code.h"

namespace fviz {

enum kFlagType {
  T_SWITCH,
  T_STRING,
  T_INT64,
  T_UINT64,
  T_FLOAT64,
};

struct FlagState {
  kFlagType type;
  bool required;
  const char* longopt;
  std::vector<std::string> values;
  void* value;
  bool has_value;
};

class FlagParser {
public:

  FlagParser();

  /**
   * Define a string flag
   */
  void defineString(
      const char* longopt,
      bool required,
      std::string* value);

  /**
   * Define a boolean flag
   */
  void defineSwitch(
      const char* longopt,
      bool* value);

  /**
   * Define a int64 flag
   */
  void defineInt64(
      const char* longopt,
      bool required,
      int64_t* value);

  /**
   * Define a uint64 flag
   */
  void defineUInt64(
      const char* longopt,
      bool required,
      uint64_t* value);

  /**
   * Define a float64 flag
   */
  void defineFloat64(
      const char* longopt,
      bool required,
      double* value);

  /**
   * Parse an argv array. This may throw an exception.
   */
  ReturnCode parseArgv(int argc, const char** argv);

  /**
   * Parse an argv array. This may throw an exception.
   */
  ReturnCode parseArgv(const std::vector<std::string>& argv);

protected:
  std::vector<FlagState> flags_;
};

}

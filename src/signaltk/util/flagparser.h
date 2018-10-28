/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <vector>
#include "return_code.h"

namespace signaltk {

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

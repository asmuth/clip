/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
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

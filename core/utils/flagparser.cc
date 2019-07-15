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
#include <string>
#include <vector>
#include <assert.h>
#include "flagparser.h"

namespace fviz {

FlagParser::FlagParser() {}

void FlagParser::defineString(
    const char* longopt,
    bool required,
    std::string* value) {
  FlagState flag_state;
  flag_state.type = T_STRING;
  flag_state.required = required;
  flag_state.longopt = longopt;
  flag_state.value = static_cast<void*>(value);
  flag_state.has_value = false;
  flags_.emplace_back(flag_state);
}

void FlagParser::defineSwitch(
    const char* longopt,
    bool* value) {
  FlagState flag_state;
  flag_state.type = T_SWITCH;
  flag_state.required = false;
  flag_state.longopt = longopt;
  flag_state.value = static_cast<void*>(value);
  flag_state.has_value = false;
  flags_.emplace_back(flag_state);
}

void FlagParser::defineInt64(
    const char* longopt,
    bool required,
    int64_t* value) {
  FlagState flag_state;
  flag_state.type = T_INT64;
  flag_state.required = required;
  flag_state.longopt = longopt;
  flag_state.value = static_cast<void*>(value);
  flag_state.has_value = false;
  flags_.emplace_back(flag_state);
}

void FlagParser::defineUInt64(
    const char* longopt,
    bool required,
    uint64_t* value) {
  FlagState flag_state;
  flag_state.type = T_UINT64;
  flag_state.required = required;
  flag_state.longopt = longopt;
  flag_state.value = static_cast<void*>(value);
  flag_state.has_value = false;
  flags_.emplace_back(flag_state);
}

void FlagParser::defineFloat64(
    const char* longopt,
    bool required,
    double* value) {
  FlagState flag_state;
  flag_state.type = T_FLOAT64;
  flag_state.required = required;
  flag_state.longopt = longopt;
  flag_state.value = static_cast<void*>(value);
  flag_state.has_value = false;
  flags_.emplace_back(flag_state);
}

ReturnCode parseValue(FlagState* flag, const std::string& value) {
  switch (flag->type) {
    case T_STRING:
      *static_cast<std::string*>(flag->value) = value;
      break;

    case T_SWITCH:
      if (value == "on") {
        *static_cast<bool*>(flag->value) = true;
      } else if (value == "off") {
        *static_cast<bool*>(flag->value) = false;
      } else {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --{}=... invalid value",
            flag->longopt);
      }
      break;

    case T_INT64:
      try {
        *static_cast<int64_t*>(flag->value) = std::stoll(value);
      } catch (std::exception e) {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --{}=... invalid value",
            flag->longopt);
      }
      break;

    case T_UINT64:
      try {
        *static_cast<uint64_t*>(flag->value) = std::stoull(value);
      } catch (std::exception e) {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --{}=... invalid value",
            flag->longopt);
      }
      break;

    case T_FLOAT64:
      try {
        *static_cast<double*>(flag->value) = std::stod(value);
      } catch (std::exception e) {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --{}=... invalid value",
            flag->longopt);
      }
      break;

    default:
      return ReturnCode::errorf(
          "FLAG_ERROR",
          "flag --{}=... invalid type",
          flag->longopt);
  }

  flag->has_value = true;
  return ReturnCode::success();
}

ReturnCode FlagParser::parseArgv(int argc, const char** argv) {
  std::vector<std::string> args;
  for (int i = 0; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  return parseArgv(args);
}

ReturnCode FlagParser::parseArgv(const std::vector<std::string>& argv) {
  for (int i = 0; i < argv.size(); i++) {
    int eq_len = -1;
    FlagState* flag_ptr = nullptr;
    auto& arg = argv[i];

    if (arg.size() == 0) {
      continue;
    }

    for (auto& flag : flags_) {
      auto longopt = std::string("--") + flag.longopt;
      auto longopt_eq = std::string("--") + flag.longopt + "=";

      if (arg.size() == longopt.size() &&
          arg.compare(0, longopt.size(), longopt) == 0) {
        flag_ptr = &flag;
      }

      else if (arg.size() == longopt_eq.size() &&
          arg.compare(0, longopt_eq.size(), longopt_eq) == 0) {
        flag_ptr = &flag;
        eq_len = longopt_eq.size();
      }

      if (flag_ptr != nullptr) {
        break;
      }
    }

    if (flag_ptr == nullptr) {
      return ReturnCode::errorf(
          "FLAG_ERROR",
          "invalid flag: {}",
          arg);
    } else if (flag_ptr->type == T_SWITCH) {
      auto rc = parseValue(flag_ptr, "on");
      if (!rc.isSuccess()) {
        return rc;
      }
    } else if (eq_len > 0) {
      if (arg.size() == eq_len) {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --{}=... has no value",
            flag_ptr->longopt);
      }

      auto rc = parseValue(flag_ptr, arg.substr(eq_len));
      if (!rc.isSuccess()) {
        return rc;
      }
    } else {
      if (i + 1 < argv.size()) {
        auto rc = parseValue(flag_ptr, argv[++i]);
        if (!rc.isSuccess()) {
          return rc;
        }
      } else if (flag_ptr->required) {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --{} has no value",
            flag_ptr->longopt);
      }
    }
  }

  for (const auto& flag : flags_) {
    if (flag.required == true && !flag.has_value) {
      return ReturnCode::errorf(
          "FLAG_ERROR",
          "flag --{} is required",
          flag.longopt);
    }
  }

  return ReturnCode::success();
}

} // namespace fviz


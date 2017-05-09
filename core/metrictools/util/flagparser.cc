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
#include <string>
#include <vector>
#include <assert.h>
#include "flagparser.h"

FlagParser::FlagParser() : ignore_unknown_(false) {}

void FlagParser::defineFlag(
    const char* longopt,
    kFlagType type,
    bool required,
    const char* shortopt /* = NULL */,
    const char* default_value /* = NULL */) {

  FlagState flag_state;
  flag_state.type = type;
  flag_state.required = required;
  flag_state.longopt = longopt;
  flag_state.shortopt = shortopt;
  flag_state.default_value = default_value;
  flags_.emplace_back(flag_state);
}

bool FlagParser::isSet(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      return flag.default_value != nullptr || flag.values.size() > 0;
    }
  }

  return false;
}

bool FlagParser::isSetExplicit(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      return flag.values.size() > 0;
    }
  }

  return false;
}

std::string FlagParser::getString(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      assert(flag.type == T_STRING);

      if (flag.values.size() == 0) {
        if (flag.default_value != nullptr) {
          return flag.default_value;
        } else {
          break;
        }
      }

      return flag.values.back();
    }
  }

  return "";
}

std::vector<std::string> FlagParser::getStrings(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      assert(flag.type == T_STRING);

      if (flag.values.size() == 0) {
        if (flag.default_value == nullptr) {
          return std::vector<std::string>();
        } else {
          return std::vector<std::string> { flag.default_value };
        }
      }

      return flag.values;
    }
  }

  return std::vector<std::string>();
}

int64_t FlagParser::getInt(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      assert(flag.type == T_INTEGER);

      std::string flag_value_str;

      if (flag.values.size() == 0) {
        if (flag.default_value != nullptr) {
          flag_value_str = flag.default_value;
        } else {
          break;
        }
      } else {
        flag_value_str = flag.values.back();
      }

      int64_t flag_value;
      try {
        flag_value = std::stoll(flag_value_str);
      } catch (std::exception e) {
        break;
      }

      return flag_value;
    }
  }

  return 0;
}

double FlagParser::getFloat(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      assert(flag.type == T_FLOAT);

      std::string flag_value_str;

      if (flag.values.size() == 0) {
        if (flag.default_value != nullptr) {
          flag_value_str = flag.default_value;
        } else {
          break;
        }
      } else {
        flag_value_str = flag.values.back();
      }

      double flag_value;
      try {
        flag_value = std::stod(flag_value_str);
      } catch (std::exception e) {
        break;
      }

      return flag_value;
    }
  }

  return 0;
}

ReturnCode FlagParser::parseArgv(int argc, const char** argv) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
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

      else if (flag.shortopt != nullptr) {
        auto shortopt = std::string("-") + flag.shortopt;
        auto shortopt_eq = std::string("-") + flag.shortopt + "=";

        if (arg.compare(0, shortopt.size(), shortopt) == 0) {
          flag_ptr = &flag;
        }

        else if (arg.compare(0, shortopt_eq.size(), shortopt_eq) == 0) {
          flag_ptr = &flag;
          eq_len = shortopt_eq.size();
        }
      }

      if (flag_ptr != nullptr) {
        break;
      }
    }

    if (flag_ptr == nullptr) {
      argv_.push_back(arg);
    } else if (flag_ptr->type == T_SWITCH) {
      flag_ptr->values.emplace_back("true");
      flag_values_.emplace_back(FlagValue {
        .shortopt = flag_ptr->shortopt,
        .longopt = flag_ptr->longopt,
        .value = "true"
      });
    } else if (eq_len > 0) {
      if (arg.size() == eq_len) {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --$0=... has no value",
            flag_ptr->longopt);
      }

      flag_ptr->values.emplace_back(arg.substr(eq_len));
      flag_values_.emplace_back(FlagValue {
        .shortopt = flag_ptr->shortopt,
        .longopt = flag_ptr->longopt,
        .value = flag_ptr->values.back()
      });
    } else {
      if (i + 1 < argv.size()) {
        flag_ptr->values.emplace_back(argv[++i]);
      } else if (!flag_ptr->required) {
        flag_ptr->values.emplace_back();
      } else {
        return ReturnCode::errorf(
            "FLAG_ERROR",
            "flag --$0 has no value",
            flag_ptr->longopt);
      }

      flag_values_.emplace_back(FlagValue {
        .shortopt = flag_ptr->shortopt ? flag_ptr->shortopt : "",
        .longopt = flag_ptr->longopt,
        .value = flag_ptr->values.back()
      });
    }
  }

  for (const auto& flag : flags_) {
    if (flag.required == true && flag.values.size() == 0) {
      return ReturnCode::errorf(
          "FLAG_ERROR",
          "flag --$0 is required",
          flag.longopt);
    }
  }

  return ReturnCode::success();
}

const std::vector<std::string>& FlagParser::getArgv() const {
  return argv_;
}

const std::vector<FlagParser::FlagValue>& FlagParser::getFlagValues() const {
  return flag_values_;
}

void FlagParser::ignoreUnknownFlags() {
  ignore_unknown_ = true;
}


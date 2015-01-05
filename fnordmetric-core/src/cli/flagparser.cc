#include <string>
#include <vector>
#include <fnord/base/exception.h>
#include <fnord/io/outputstream.h>
#include <fnordmetric/cli/flagparser.h>

namespace fnordmetric {
namespace cli {

FlagParser::FlagParser() {}

void FlagParser::defineFlag(
    const char* longopt,
    kFlagType type,
    bool required,
    const char* shortopt /* = NULL */,
    const char* default_value /* = NULL */,
    const char* description /* = NULL */,
    const char* placeholder /* = NULL */) {

  FlagState flag_state;
  flag_state.type = type;
  flag_state.required = required;
  flag_state.longopt = longopt;
  flag_state.shortopt = shortopt;
  flag_state.default_value = default_value;
  flag_state.description = description;
  flag_state.placeholder = placeholder;
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

std::string FlagParser::getString(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      if (flag.type != T_STRING) {
        RAISE(kFlagError, "flag '%s' is not a string", longopt);
      }

      if (flag.values.size() == 0) {
        if (flag.default_value != nullptr) {
          return flag.default_value;
        }

        RAISE(kFlagError, "flag '%s' is not set", longopt);
      }

      return flag.values.back();
    }
  }

  RAISE(kFlagError, "flag '%s' is not set", longopt);
}

int64_t FlagParser::getInt(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      if (flag.type != T_INTEGER) {
        RAISE(kFlagError, "flag '%s' is not an integer", longopt);
      }

      std::string flag_value_str;

      if (flag.values.size() == 0) {
        if (flag.default_value != nullptr) {
          flag_value_str = flag.default_value;
        } else {
          RAISE(kFlagError, "flag '%s' is not set", longopt);
        }
      } else {
        flag_value_str = flag.values.back();
      }

      int64_t flag_value;
      try {
        flag_value = std::stoi(flag_value_str);
      } catch (std::exception e) {
        RAISE(
            kFlagError,
            "flag '%s' value '%s' is not a valid integer",
            longopt,
            flag.values.back().c_str());
      }

      return flag_value;
    }
  }

  RAISE(kFlagError, "flag '%s' is not set", longopt);
}

void FlagParser::parseArgv(int argc, const char** argv) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  parseArgv(args);
}

// FIXPAUL optimize with hashmap?
void FlagParser::parseArgv(const std::vector<std::string>& argv) {
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

      if (arg.compare(0, longopt.size(), longopt) == 0) {
        flag_ptr = &flag;
      }

      else if (arg.compare(0, longopt_eq.size(), longopt_eq) == 0) {
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
    } else if (eq_len > 0) {
      if (arg.size() == eq_len) {
        RAISE(kFlagError, "flag --%s=... has no value", flag_ptr->longopt);
      }

      flag_ptr->values.emplace_back(arg.substr(eq_len));
    } else {
      if (i + 1 >= argv.size()) {
        RAISE(kFlagError, "flag --%s has no value", flag_ptr->longopt);
      }

      flag_ptr->values.emplace_back(argv[++i]);
    }
  }

  for (const auto& flag : flags_) {
    if (flag.required == true && flag.values.size() == 0) {
      RAISE(kFlagError, "flag --%s is required", flag.longopt);
    }
  }
}

const std::vector<std::string>& FlagParser::getArgv() const {
  return argv_;
}

void FlagParser::printUsage(fnord::io::OutputStream* target) const {
  for (const auto& flag : flags_) {
    if (flag.shortopt == nullptr) {
      target->printf("    --%-26.26s", flag.longopt);
    } else {
      target->printf("    -%s, --%-12.12s", flag.shortopt, flag.longopt);
    }

    const char* placeholder = nullptr;
    if (flag.placeholder == nullptr) {
      switch (flag.type) {
        case T_STRING:
          placeholder = "<string>";
          break;
        case T_INTEGER:
          placeholder = "<int>";
          break;
        case T_FLOAT:
          placeholder = "<float>";
          break;
        case T_SWITCH:
          placeholder = "";
          break;
      }
    } else {
      placeholder = flag.placeholder;
    }
    target->printf("%-12.12s", placeholder);

    if (flag.description != nullptr) {
      target->printf("%s\n", flag.description);
    } else {
      target->printf("\n");
    }
  }
}

}
}

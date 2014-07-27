#include <string>
#include <vector>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/runtimeexception.h>

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
      return flag.values.size() > 0;
    }
  }

  return false;
}

std::string FlagParser::getString(const char* longopt) const {
  for (auto& flag : flags_) {
    if (flag.longopt == longopt) {
      if (flag.type != T_STRING) {
        RAISE(FlagError, "flag '%s' is not a string", longopt);
      }

      return flag.values.back();
    }
  }

  RAISE(FlagError, "flag '%s' is not set", longopt);
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
        RAISE(FlagError, "flag --%s=... has no value", flag_ptr->longopt);
      }

      flag_ptr->values.emplace_back(arg.substr(eq_len));
    } else {
      if (i + 1 >= argv.size()) {
        RAISE(FlagError, "flag --%s has no value", flag_ptr->longopt);
      }

      flag_ptr->values.emplace_back(argv[++i]);
    }
  }

  for (const auto& flag : flags_) {
    if (flag.required == true && flag.values.size() == 0) {
      RAISE(FlagError, "flag --%s is required", flag.longopt);
    }
  }
}

const std::vector<std::string>& FlagParser::getArgv() const {
  return argv_;
}

void FlagParser::printUsage(util::OutputStream* target) const {
  target->printf("usage: fnordmetric [options] [file.sql]\n");

  target->printf("\noptions:\n");
  for (const auto& flag : flags_) {
    if (flag.shortopt == nullptr) {
      target->printf("    --%-16.16s", flag.longopt);
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

  target->printf("\nexamples:\n");
  target->printf("    $ fnordmeric -f svg -o out.svg myquery.sql\n");
  target->printf("    $ fnordmeric -f svg - < myquery.sql > out.svg\n");
}

}
}

#include <string>
#include <vector>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace cli {

FlagParser::FlagParser() {}

void FlagParser::defineFlag(
    kFlagType type,
    bool required,
    const char* shortopt,
    const char* longopt /* = NULL */,
    const char* default_value /* = NULL */,
    const char* description /* = NULL */,
    const char* placeholder /* = NULL */) {

  FlagState flag_state;
  flag_state.type = type;
  flag_state.required = required;
  flag_state.shortopt = shortopt;
  flag_state.longopt = longopt;
  flag_state.default_value = default_value;
  flag_state.description = description;
  flag_state.placeholder = placeholder;
  flags_.emplace_back(flag_state);
}

// FIXPAUL optimize with hashmap?
void FlagParser::parseArgv(const std::vector<std::string>& argv) {
  for (int i = 0; i < argv.size(); i++) {
    auto& arg = argv[i];

    if (arg.size() == 0) {
      continue;
    }

    int eq_len = -1;
    FlagState* flag_ptr = nullptr;

    for (auto& flag : flags_) {
      auto shortopt = std::string("-") + flag.shortopt;
      auto shortopt_eq = std::string("-") + flag.shortopt + "=";

      if (arg.compare(0, shortopt.size(), shortopt)) {
        flag_ptr = &flag;
      }

      else if (arg.compare(0, shortopt_eq.size(), shortopt_eq)) {
        flag_ptr = &flag;
        eq_len = shortopt_eq.size();
      }

      else if (flag.longopt != nullptr) {
        auto longopt = std::string("--") + flag.longopt;
        auto longopt_eq = std::string("--") + flag.longopt + "=";

        if (arg.compare(0, longopt.size(), longopt)) {
          flag_ptr = &flag;
        }

        else if (arg.compare(0, longopt_eq.size(), longopt_eq)) {
          flag_ptr = &flag;
          eq_len = longopt_eq.size();
        }
      }

      if (flag_ptr != nullptr) {
        break;
      }
    }

    if (flag_ptr == nullptr) {
      argv_.push_back(arg);
    } else {

    }
  }
}

const std::vector<std::string>& FlagParser::getArgv() const {
  return argv_;
}

}
}

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
#include "flagparser.h"

#include <string>
#include <vector>
#include <assert.h>
#include <getopt.h>

namespace clip {

void flags_add_string(
      FlagList* flags,
      char shortopt,
      const std::string& longopt,
      std::string* value) {
  FlagInfo flag;
  flag.opt_short = shortopt;
  flag.opt_long = longopt;
  flag.has_arg = true;

  flag.callback = [value] (auto v) {
    *value = v ? std::string(v) : std::string();
  };

  flags->push_back(flag);
}

void flags_add_stringv(
      FlagList* flags,
      char shortopt,
      const std::string& longopt,
      std::vector<std::string>* values) {
  FlagInfo flag;
  flag.opt_short = shortopt;
  flag.opt_long = longopt;
  flag.has_arg = true;

  flag.callback = [values] (auto v) {
    values->push_back(v ? std::string(v) : std::string());
  };

  flags->push_back(flag);
}

void flags_add_switch(
      FlagList* flags,
      char shortopt,
      const std::string& longopt,
      bool* value) {
  FlagInfo flag;
  flag.opt_short = shortopt;
  flag.opt_long = longopt;
  flag.has_arg = false;

  flag.callback = [value] (auto _) {
    *value = true;
  };

  flags->push_back(flag);
}

ReturnCode flags_parse(
    const FlagList& flags,
    int argc,
    char** argv,
    std::vector<std::string>* args_out) {
  // prepapre long opts
  std::vector<struct option> opts_long(flags.size() + 1);
  opts_long[flags.size()] = {0, 0, 0, 0};

  for (size_t i = 0; i < flags.size(); ++i) {
    opts_long[i] = {
      flags[i].opt_long.c_str(),
      flags[i].has_arg ? 1 : 0,
      0,
      0
    };
  }

  // prepare short opts
  std::string opts_short;
  for (const auto& f : flags) {
    if (f.opt_short == 0) {
      continue;
    }

    opts_short.push_back(f.opt_short);

    if (f.has_arg) {
      opts_short.push_back(':');
    }
  }

  // call getopt
  for (;;) {
    int opt_long = 0;
    int opt_short = getopt_long(
        argc,
        argv,
        opts_short.c_str(),
        opts_long.data(),
        &opt_long);

    if (opt_short == -1) {
      break;
    }

    const FlagInfo* flag = nullptr;
    if (opt_short == 0) {
      flag = &flags[opt_long];
    } else {
      for (const auto& f : flags) {
        if (opt_short == f.opt_short) {
          flag = &f;
          break;
        }
      }
    }

    if (!flag) {
      return error(ERROR, "invalid option");
    }

    flag->callback(optarg);
  }

  // copy unparsed arguments
  if (args_out) {
    for (size_t i = optind; i < argc; ++i) {
      args_out->push_back(argv[i]);
    }
  }

  return OK;
}

} // namespace clip


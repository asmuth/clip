/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fnordmetric/cli/cli.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/environment.h>

using namespace fnordmetric;

int main(int argc, char** argv) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  try {
    cli::CLI::parseArgs(env(), args);
    return cli::CLI::executeSafely(env());
  } catch (std::exception e) {
    fprintf(
        stderr,
        "FnordMetric crashed :( -- Please report a bug at "
        "github.com/paulasmuth/fnordmetric\n\n");

    auto runtime_exception = dynamic_cast<util::RuntimeException*>(&e);
    if (runtime_exception == nullptr) {
      fprintf(stderr, "Aborting...\n");
      abort(); // core dump if enabled
    } else {
      runtime_exception->debugPrint();
      return 1;
    }
  }
}


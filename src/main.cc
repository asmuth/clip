/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fnordmetric/cli/cli.h>
#include <fnordmetric/util/outputstream.h>

using namespace fnordmetric;

int main(int argc, char** argv) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  try {
    auto stderr_stream = util::OutputStream::getStderr();
    return cli::CLI::executeSafely(args, stderr_stream.get());
  } catch (std::exception e) {
    fprintf(
        stderr,
        "FnordMetric crashed :( -- Please report a bug at "
        "github.com/paulasmuth/fnordmetric");

    auto runtime_exception = dynamic_cast<util::RuntimeException*>(&e);
    if (runtime_exception != nullptr) {
      runtime_exception->debugPrint();
    }

    return 1;
  }
}


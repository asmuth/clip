/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <vector>
#include <string>
#include <fnordmetric/cli/cli.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace cli {

void CLI::execute(std::vector<std::string> args) {
  std::unique_ptr<util::InputStream> input;

  switch (args.size()) {
    case 0:
      input = std::move(util::InputStream::getStdin());
      break;
    case 1:
      input = std::move(util::FileInputStream::openFile(args[0]));
      break;
    default:
      RAISE(UsageError);
  }

}

}
}

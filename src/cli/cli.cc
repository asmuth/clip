/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <vector>
#include <string>
#include <fnordmetric/cli/cli.h>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace cli {

void CLI::execute(const cli::FlagParser& flag_parser) {
  std::unique_ptr<util::InputStream> input;

  const auto& args = flag_parser.getArgv();

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

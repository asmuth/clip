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
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace cli {

void CLI::execute(const cli::FlagParser& flag_parser) {
  bool verbose = flag_parser.isSet("verbose");
  auto err = util::OutputStream::getStderr();
  const auto& args = flag_parser.getArgv();

  /* web / cgi mode */

  /* execute query */

  /* open input stream */
  std::unique_ptr<util::InputStream> input;
  switch (args.size()) {
    case 0:
      if (verbose) {
        err->printf("[INFO] input from stdin %s\n");
      }

      input = std::move(util::InputStream::getStdin());
      break;
    case 1:
      if (verbose) {
        err->printf("[INFO] input file: %s\n", args[0].c_str());
      }

      input = std::move(util::FileInputStream::openFile(args[0]));
      break;
    default:
      RAISE(UsageError);
  }

  /* open output stream */
  std::unique_ptr<util::OutputStream> output;
  if (flag_parser.isSet("output")) {
    auto output_file = flag_parser.getString("output");

    if (verbose) {
      err->printf("[INFO] output file: %s\n", output_file.c_str());
    }

    output = std::move(util::FileOutputStream::openFile(output_file));
  } else {
    if (verbose) {
      err->printf("[INFO] output to stdout\n");
    }
    output = std::move(util::OutputStream::getStdout());
  }

  query::QueryService query_service;
  query_service.executeQuery(
      input.get(),
      query::QueryService::FORMAT_SVG,
      output.get());
}

}
}

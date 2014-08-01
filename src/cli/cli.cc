/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

FlagParser CLI::getDefaultFlagParser() {
  FlagParser flag_parser;

  // Execute queries from the commandline:
  flag_parser.defineFlag(
      "format",
      FlagParser::T_STRING,
      false,
      "f",
      "human",
      "The output format (svg,csv,human)",
      "<format>");

  flag_parser.defineFlag(
      "output",
      FlagParser::T_STRING,
      false,
      "o",
      NULL,
      "Write output to a file",
      "<format>");

  // Start a user interface:
  flag_parser.defineFlag(
      "repl",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Start an interactive readline shell",
      NULL);

  flag_parser.defineFlag(
      "web",
      FlagParser::T_INTEGER,
      false,
      NULL,
      NULL,
      "Start a web interface on this port",
      "<port>");

  flag_parser.defineFlag(
      "cgi",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Run as CGI script");

  flag_parser.defineFlag(
      "verbose",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Be verbose");

  flag_parser.defineFlag(
      "help",
      FlagParser::T_SWITCH,
      false,
      "h",
      NULL,
      "You are reading it...");

  return flag_parser;
}

int CLI::executeSafely(
      const std::vector<std::string>& argv,
      util::OutputStream* error_stream) {
  bool verbose = true;
  auto flag_parser = getDefaultFlagParser();

  try {
    flag_parser.parseArgv(argv);
    verbose = flag_parser.isSet("verbose");
    execute(flag_parser, error_stream);
  } catch (util::RuntimeException e) {
    if (e.getTypeName() == "UsageError") {
      flag_parser.printUsage(error_stream);
      return 1;
    }

    error_stream->printf("fatal error: %s\n", e.getMessage().c_str());

    if (verbose) {
      e.debugPrint();
    }

    return 1;
  }

  return 0;
}

void CLI::execute(
      const std::vector<std::string>& argv,
      util::OutputStream* error_stream) {
  /* parse flags */
  auto flag_parser = getDefaultFlagParser();
  flag_parser.parseArgv(argv);
  bool verbose = flag_parser.isSet("verbose");
  const auto& args = flag_parser.getArgv();
  execute(flag_parser, error_stream);
}

void CLI::execute(
      const FlagParser& flag_parser,
      util::OutputStream* error_stream) {
  const auto& args = flag_parser.getArgv();
  bool verbose = flag_parser.isSet("verbose");

  /* web / cgi mode */

  /* execute query */

  /* open input stream */
  std::unique_ptr<util::InputStream> input;
  if (args.size() == 1) {
    if (args[0] == "-") {
      if (verbose) {
        error_stream->printf("[INFO] input from stdin %s\n");
      }

      input = std::move(util::InputStream::getStdin());
    } else {
      if (verbose) {
        error_stream->printf("[INFO] input file: %s\n", args[0].c_str());
      }

      input = std::move(util::FileInputStream::openFile(args[0]));
    }
  } else {
    RAISE(UsageError);
  }

  /* open output stream */
  std::unique_ptr<util::OutputStream> output;
  if (flag_parser.isSet("output")) {
    auto output_file = flag_parser.getString("output");

    if (verbose) {
      error_stream->printf("[INFO] output file: %s\n", output_file.c_str());
    }

    output = std::move(util::FileOutputStream::openFile(output_file));
  } else {
    if (verbose) {
      error_stream->printf("[INFO] output to stdout\n");
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

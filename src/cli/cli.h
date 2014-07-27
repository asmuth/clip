/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CLI_H
#define _FNORDMETRIC_CLI_H
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace util {
class OutputStream;
}
namespace cli {
class FlagParser;

class CLI {
public:
  struct UsageError : public fnordmetric::util::RuntimeException {
    UsageError() : RuntimeException("usage error") {}
  };

  static FlagParser getDefaultFlagParser();

  /**
   * Execute a command line. Should not throw an exception!
   */
  static int executeSafely(
      const std::vector<std::string>& argv,
      util::OutputStream* error_stream);

  /**
   * Execute a command line. May throw an exception!
   */
  static void execute(
      const std::vector<std::string>& argv,
      util::OutputStream* error_stream);

  /**
   * Execute a command line.  May throw an exception!
   */
  static void execute(
      const FlagParser& flag_parser,
      util::OutputStream* error_stream);
};

}
}
#endif

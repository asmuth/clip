/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

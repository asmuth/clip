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
namespace cli {

class CLI {
public:
  struct UsageError : public fnordmetric::util::RuntimeException {
    UsageError() : RuntimeException("usage error") {}
  };

  /**
   * Execute a command line
   *
   * May throw an exception!
   */
  static void execute(std::vector<std::string> args);
}
;

}
}
#endif

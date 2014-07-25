/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdio.h>
#include <stdarg.h>
#include "runtimeexception.h"

namespace fnordmetric {
namespace util {

RuntimeException::RuntimeException(
    const char* file,
    int line,
    const char* func,
    const char* message,
    ...) :
    file_(file),
    line_(line),
    func_(func) {
  va_list args;
  va_start(args, args);
  vsnprintf(message_, sizeof(message_), message, args);
  va_end(args);
}

void RuntimeException::debugPrint() const {
  fprintf(
      stderr,
      "RuntimeException[1234]: %s\n"
      "    in %s\n"
      "    in %s:%i\n",
      message_,
      func_,
      file_,
      line_);
}

}
}

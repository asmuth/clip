/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "runtimeexception.h"

namespace fnordmetric {
namespace util {

RuntimeException::RuntimeException(
    int type_id,
    const char* type_human,
    const char* file,
    int line,
    const char* func,
    int posix_errno,
    const char* message,
    ...) :
    type_id_(type_id),
    type_human_(type_human),
    file_(file),
    line_(line),
    func_(func) {
  va_list args;
  va_start(args, message);
  int pos = vsnprintf(message_, sizeof(message_), message, args);
  va_end(args);

  if (pos < 0) {
    pos = 0;
  }

  if (errno > 0) {
    snprintf(message_ + pos, sizeof(message_) - pos, ": ");
    pos += 2;
    strerror_r(errno, message_ + pos, sizeof(message_) - pos);
  }
}

void RuntimeException::debugPrint() const {
  fprintf(
      stderr,
      "RuntimeException[%s] (%i): %s\n"
      "    in %s\n"
      "    in %s:%i\n",
      type_human_,
      type_id_,
      message_,
      func_,
      file_,
      line_);
}

}
}

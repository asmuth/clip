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
    const char* message,
    ...) :
    namespace_id_(0),
    type_id_(0),
    type_human_(NULL),
    file_(NULL),
    line_(0),
    func_(NULL) {
  va_list args;
  va_start(args, message);
  int pos = vsnprintf(message_, sizeof(message_), message, args);
  va_end(args);

  if (pos < 0) {
    pos = 0;
  }

  int posix_errno = -1;
  if (posix_errno > 0) {
    snprintf(message_ + pos, sizeof(message_) - pos, ": ");
    pos += 2;
    strerror_r(posix_errno, message_ + pos, sizeof(message_) - pos);
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

std::string RuntimeException::getMessage() const {
  return std::string(message_);
}

}
}

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
    type_name_(nullptr),
    file_(nullptr),
    line_(0),
    func_(nullptr) {
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

RuntimeException::RuntimeException(
    const RuntimeException& other) :
    type_name_(other.type_name_),
    file_(other.file_),
    line_(other.line_),
    func_(other.func_) {
  strncpy(message_, other.message_, sizeof(message_));
}

RuntimeException RuntimeException::setSource(
    const char* file,
    int line,
    const char* func) {
  file_ = file;
  line_ = line;
  func_ = func;
  return *this;
}

RuntimeException RuntimeException::setTypeName(const char* type_name) {
  type_name_ = type_name;
  return *this;
}

void RuntimeException::debugPrint() const {
  const char* type_name =
      type_name_ == nullptr ? "RuntimeException" : type_name_;

  fprintf(
      stderr,
      "\n%s: %s\n"
      "    in %s\n"
      "    in %s:%i\n",
      type_name,
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

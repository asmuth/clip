/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

RuntimeException RuntimeException::setErrno(int posix_errno) {
  if (posix_errno > 0) {
    size_t pos = strlen(message_);
    snprintf(message_ + pos, sizeof(message_) - pos, ": ");
    pos += 2;
    strerror_r(posix_errno, message_ + pos, sizeof(message_) - pos);
  }

  return *this;
}

void RuntimeException::appendMessage(const char* message, ...) {
  va_list args;
  va_start(args, message);
  size_t pos = strlen(message_);
  vsnprintf(message_ + pos, sizeof(message_) - pos, message, args);
  va_end(args);
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

std::string RuntimeException::getTypeName() const {
  return std::string(type_name_);
}

}
}

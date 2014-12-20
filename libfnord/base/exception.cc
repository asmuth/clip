/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fnord/base/exception.h>

namespace fnord {

Exception::Exception() :
    type_name_(nullptr),
    file_(nullptr),
    line_(0),
    func_(nullptr) {
  *message_ = 0;
}

Exception::Exception(
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

Exception::Exception(
    const std::string& message) :
    type_name_(nullptr),
    file_(nullptr),
    line_(0),
    func_(nullptr) {
  strncpy(message_, message.c_str(), sizeof(message_));
}

Exception::Exception(
    const Exception& other) :
    type_name_(other.type_name_),
    file_(other.file_),
    line_(other.line_),
    func_(other.func_) {
  strncpy(message_, other.message_, sizeof(message_));
}

Exception Exception::setSource(
    const char* file,
    int line,
    const char* func) {
  file_ = file;
  line_ = line;
  func_ = func;
  return *this;
}

Exception Exception::setTypeName(const char* type_name) {
  type_name_ = type_name;
  return *this;
}

Exception Exception::setErrno(int posix_errno) {
  if (posix_errno > 0) {
    char buf[4096];
    char* errstr = buf;

#ifdef _GNU_SOURCE
    errstr = strerror_r(posix_errno, buf, sizeof(buf));
#else
    strerror_r(posix_errno, buf, sizeof(buf));
#endif

    size_t pos = strlen(message_);
    snprintf(message_ + pos, sizeof(message_) - pos, ": %s", errstr);
  }

  return *this;
}

void Exception::appendMessage(const char* message, ...) {
  va_list args;
  va_start(args, message);
  size_t pos = strlen(message_);
  vsnprintf(message_ + pos, sizeof(message_) - pos, message, args);
  va_end(args);
}

void Exception::debugPrint(io::OutputStream* os /* = nullptr */) const {
  const char* type_name =
      type_name_ == nullptr ? "Exception" : type_name_;

  std::unique_ptr<io::OutputStream> os_local;
  if (os == nullptr) {
    os_local = io::OutputStream::getStderr();
    os = os_local.get();
  }

  os->printf(
      "%s: %s\n"
      "    in %s\n"
      "    in %s:%i\n",
      type_name,
      message_,
      func_,
      file_,
      line_);
}

bool Exception::ofType(const char* type_name) {
  return strcmp(type_name_, type_name) == 0;
}

std::string Exception::getMessage() const {
  return std::string(message_);
}

std::string Exception::getTypeName() const {
  return std::string(type_name_);
}

std::string Exception::method() const {
  return std::string(func_);
}

std::string Exception::file() const {
  return std::string(file_);
}

int Exception::line() const {
  return line_;
}

} // namespace fnord


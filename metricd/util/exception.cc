/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <metricd/util/exception.h>
#include <metricd/util/outputstream.h>

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

#if !_GNU_SOURCE || (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
    strerror_r(posix_errno, buf, sizeof(buf));
#else
    errstr = strerror_r(posix_errno, buf, sizeof(buf));
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

void Exception::debugPrint(OutputStream* os /* = nullptr */) const {
  const char* type_name =
      type_name_ == nullptr ? "Exception" : type_name_;

  std::unique_ptr<OutputStream> os_local;
  if (os == nullptr) {
    os_local = OutputStream::getStderr();
    os = os_local.get();
  }

  os->write(
      StringUtil::format(
          "$0: $1\n"
          "    in $2\n"
          "    in $3:$4\n",
          type_name,
          message_,
          func_,
          file_,
          line_));
}

const char* Exception::what() const noexcept {
  return message_;
}

bool Exception::ofType(const char* type_name) {
  return strcmp(type_name_, type_name) == 0;
}

std::string Exception::getMessage() const {
  return std::string(message_);
}

ExceptionType Exception::getType() const {
  return type_name_;
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


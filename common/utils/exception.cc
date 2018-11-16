/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "exception.h"

namespace plotfx {

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

} // namespace plotfx


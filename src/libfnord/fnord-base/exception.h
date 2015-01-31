/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_EXCEPTION_H
#define _FNORD_BASE_EXCEPTION_H
#include <errno.h>
#include <exception>
#include <string>
#include "fnord-base/stringutil.h"
#include "fnord-base/stdtypes.h"
#include "fnord-base/io/outputstream.h"

const char kBufferOverflowError[] = "BufferOverflowError";
const char kConcurrentModificationError[] = "ConcurrentModificationError";
const char kDivideByZeroError[] = "DivideByZeroError";
const char kFlagError[] = "FlagError";
const char kIOError[] = "IOError";
const char kIllegalArgumentError[] = "IOIllegalArgument";
const char kIllegalFormatError[] = "IllegalFormat";
const char kIllegalStateError[] = "IllegalState";
const char kIndexError[] = "IndexError";
const char kInvalidOptionError[] = "InvalidOptionError";
const char kKeyError[] = "KeyError";
const char kMallocError[] = "MallocError";
const char kNoSuchMethodError[] = "NoSuchMethodError";
const char kNotImplementedError[] = "NotImplementedError";
const char kNotYetImplementedError[] = "NotYetImplementedError";
const char kNullPointerError[] = "NullPointerError";
const char kParseError[] = "ParseError";
const char kRangeError[] = "RangeError";
const char kReflectionError[] = "kReflectionError";
const char kResolveError[] = "kResolveError";
const char kRPCError[] = "kRPCError";
const char kRuntimeError[] = "RuntimeError";
const char kTypeError[] = "TypeError";
const char kUsageError[] = "UsageError";
const char kVersionMismatchError[] = "VersionMismatchError";
const char kWouldBlockError[] = "WouldBlockError";
const char kFutureError[] = "FutureError";

#define RAISE_EXCEPTION(E) \
    throw (E).setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__); while(0) {}

#define RAISE(E, ...) \
    RAISE_EXCEPTION( \
        fnord::Exception( __VA_ARGS__).setTypeName(E)); \
        while(0) {}

#define RAISEF(E, ...) \
    RAISE_EXCEPTION( \
        fnord::Exception( \
            fnord::StringUtil::format(__VA_ARGS__)).setTypeName(E)); \
        while(0) {}

#define RAISE_ERRNO(E, ...) \
    { \
      int e = errno; \
      RAISE_EXCEPTION( \
          fnord::Exception( \
              __VA_ARGS__).setTypeName(E).setErrno(e)); \
    }

namespace fnord {

using StandardException = std::exception;

class Exception : public StandardException {
public:
  Exception();
  Exception(const char* message, ...);
  Exception(const std::string& message);
  Exception(const Exception& other);
  Exception& operator=(const Exception& other) = delete;

  void debugPrint(OutputStream* os = nullptr) const;
  std::string getMessage() const;
  std::string getTypeName() const;
  std::string method() const;
  std::string file() const;
  int line() const;

  void appendMessage(const char* message, ...);

  Exception setSource(const char* file, int line, const char* func);
  Exception setTypeName(const char* type_name);
  bool ofType(const char* type_name);
  Exception setErrno(int posix_errno);

private:
  const char* type_name_;
  const char* file_;
  int line_;
  const char* func_;
  char message_[1024];
};

} // namespace fnord

#endif

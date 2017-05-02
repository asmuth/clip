/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <errno.h>
#include <exception>
#include <string>
#include "stringutil.h"
#include "stdtypes.h"

const char kAccessDeniedError[] = "AccessDeniedError";
const char kAssertionError[] = "AssertionError";
const char kBufferOverflowError[] = "BufferOverflowError";
const char kEncodingError[] = "EncodingError";
const char kCancelledError[] = "CancelledError";
const char kConcurrentModificationError[] = "ConcurrentModificationError";
const char kDivideByZeroError[] = "DivideByZeroError";
const char kEOFError[] = "EOFError";
const char kFlagError[] = "FlagError";
const char kIOError[] = "IOError";
const char kIllegalArgumentError[] = "IllegalArgument";
const char kIllegalFormatError[] = "IllegalFormat";
const char kIllegalStateError[] = "IllegalState";
const char kIndexError[] = "IndexError";
const char kInvalidOptionError[] = "InvalidOptionError";
const char kKeyError[] = "KeyError";
const char kMallocError[] = "MallocError";
const char kNoSuchMethodError[] = "NoSuchMethodError";
const char kNotImplementedError[] = "NotImplementedError";
const char kNotFoundError[] = "NotFoundError";
const char kNotYetImplementedError[] = "NotYetImplementedError";
const char kNullPointerError[] = "NullPointerError";
const char kParseError[] = "ParseError";
const char kPermissionDenied[] = "PermissionDeniedError";
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
        Exception( __VA_ARGS__).setTypeName(E)); \
        while(0) {}

#define RAISEF(E, ...) \
    RAISE_EXCEPTION( \
        Exception( \
           StringUtil::format(__VA_ARGS__)).setTypeName(E)); \
        while(0) {}

#define RAISE_ERRNO(E, ...) \
    { \
      int e = errno; \
      RAISE_EXCEPTION( \
          Exception( \
              __VA_ARGS__).setTypeName(E).setErrno(e)); \
    }

#define RCHECK(P, ...) \
    if (!(P)) { RAISE(kAssertionError, __VA_ARGS__) } \
    while(0) {}

#define RCHECKF(P, ...) \
    if (!(P)) { RAISE(kAssertionError, StringUtil::format(__VA_ARGS__)) } \
    while(0) {}

#ifdef NDEBUG
#define RDCHECK(...) ((void) 0)
#define RDCHECKF(...) ((void) 0)
#else
#define RDCHECK(...) RCHECK(__VA_ARGS__)
#define RDCHECKF(...) RCHECKF(__VA_ARGS__)
#endif

#define __brk raise(SIGTRAP); while (0) {}

class OutputStream;

using StandardException = std::exception;
using ExceptionType = const char*;

class Exception : public StandardException {
public:
  Exception();
  Exception(ExceptionType message, ...);
  Exception(const std::string& message);
  Exception(const Exception& other);
  Exception& operator=(const Exception& other) = delete;

  void debugPrint(OutputStream* os = nullptr) const;
  std::string getMessage() const;
  ExceptionType getType() const;
  std::string getTypeName() const;
  std::string method() const;
  std::string file() const;
  int line() const;

  void appendMessage(const char* message, ...);

  Exception setSource(const char* file, int line, const char* func);
  Exception setTypeName(const char* type_name);
  bool ofType(const char* type_name);
  Exception setErrno(int posix_errno);

  const char* what() const noexcept override;

private:
  const char* type_name_;
  const char* file_;
  int line_;
  const char* func_;
  char message_[1024];
};


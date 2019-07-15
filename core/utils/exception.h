/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _FNORD_BASE_EXCEPTION_H
#define _FNORD_BASE_EXCEPTION_H
#include <errno.h>
#include <exception>
#include <string>
#include "utils/stringutil.h"
#include "utils/stdtypes.h"
#include "utils/outputstream.h"

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
        fviz::Exception( __VA_ARGS__).setTypeName(E)); \
        while(0) {}

#define RAISEF(E, ...) \
    RAISE_EXCEPTION( \
        fviz::Exception( \
            fviz::fmt::format(__VA_ARGS__)).setTypeName(E)); \
        while(0) {}

#define RAISE_ERRNO(E, ...) \
    { \
      int e = errno; \
      RAISE_EXCEPTION( \
          fviz::Exception( \
              __VA_ARGS__).setTypeName(E).setErrno(e)); \
    }

namespace fviz {

using StandardException = std::exception;

class Exception : public StandardException {
public:
  Exception();
  Exception(const char* message, ...);
  Exception(const std::string& message);
  Exception(const Exception& other);
  Exception& operator=(const Exception& other) = delete;

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

} // namespace fviz

#endif

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/inspect.h"
#include "fnord/base/exception.h"
#include "fnord/base/status.h"

namespace fnord {

Status Status::success() {
  return Status(eSuccess);
}

Status::Status(kStatusType type) : type_(type) {}

Status::Status(
    kStatusType type,
    const std::string& message) :
    type_(type),
    message_(message) {}

bool Status::isError() const {
  return type_ != eSuccess;
}

bool Status::isSuccess() const {
  return type_ == eSuccess;
}

kStatusType Status::type() const {
  return type_;
}

const std::string& Status::message() const {
  return message_;
}

template <>
std::string inspect<Status>(const Status& value) {
  return StringUtil::format("<status:$0 $1>", value.type(), value.message());
}

template <>
std::string StringUtil::toString<kStatusType>(kStatusType value) {
  switch (value) {
    case eSuccess: return "Success";
    case eBufferOverflowError: return "BufferOverflowError";
    case eConcurrentModificationError: return "ConcurrentModificationError";
    case eDivideByZeroError: return "DivideByZeroError";
    case eFlagError: return "FlagError";
    case eIOError: return "IOError";
    case eIllegalArgumentError: return "IllegalArgumentError";
    case eIllegalFormatError: return "IllegalFormatError";
    case eIllegalStateError: return "IllegalStateError";
    case eIndexError: return "IndexError";
    case eInvalidOptionError: return "InvalidOptionError";
    case eKeyError: return "KeyError";
    case eMallocError: return "MallocError";
    case eNoSuchMethodError: return "NoSuchMethodError";
    case eNotImplementedError: return "NotImplementedError";
    case eNotYetImplementedError: return "NotYetImplementedError";
    case eNullPointerError: return "NullPointerError";
    case eParseError: return "ParseError";
    case eRangeError: return "RangeError";
    case eReflectionError: return "ReflectionError";
    case eResolveError: return "ResolveError";
    case eRPCError: return "RPCError";
    case eRuntimeError: return "RuntimeError";
    case eTypeError: return "TypeError";
    case eUsageError: return "UsageError";
    case eVersionMismatchError: return "VersionMismatchError";
    case eWouldBlockError: return "WouldBlockError";
  }
}

void Status::raiseIfError() const {
  if (isError()) {
    RAISE(StringUtil::toString(type_).c_str(), message_);
  }
}

}

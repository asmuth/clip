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
#include "fnordmetric/util/inspect.h"
#include "fnordmetric/util/exception.h"
#include "fnordmetric/util/status.h"

Status Status::success() {
  return Status(eSuccess);
}

Status::Status(kStatusType type) : type_(type) {}

Status::Status(const std::exception& e) {
  try {
    auto rte = dynamic_cast<const Exception&>(e);
    type_ = eRuntimeError; // FIXPAUL
    message_ = StringUtil::format(
        "$0: $1",
        rte.getTypeName(),
        rte.getMessage());
  } catch (const std::exception& cast_error) {
    type_ = eForeignError;
    message_ = e.what();
  }
}

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
std::string StringUtil::toString<Status>(Status value) {
  return inspect(value);
}

template <>
std::string StringUtil::toString<kStatusType>(kStatusType value) {
  switch (value) {
    case eSuccess: return "Success";
    case eBufferOverflowError: return "BufferOverflowError";
    case eConcurrentModificationError: return "ConcurrentModificationError";
    case eDivideByZeroError: return "DivideByZeroError";
    case eForeignError: return "ForeignError";
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
    default: return "RuntimeError";
  }
}

void Status::raiseIfError() const {
  if (isError()) {
    RAISE(kRuntimeError, message_);
  }
}

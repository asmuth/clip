/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BASE_STATUS_H
#define _FNORD_BASE_STATUS_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>

namespace fnord {

enum kStatusType {
  eSuccess,
  eBufferOverflowError,
  eConcurrentModificationError,
  eDivideByZeroError,
  eFlagError,
  eIOError,
  eIllegalArgumentError,
  eIllegalFormatError,
  eIllegalStateError,
  eIndexError,
  eInvalidOptionError,
  eKeyError,
  eMallocError,
  eNoSuchMethodError,
  eNotImplementedError,
  eNotYetImplementedError,
  eNullPointerError,
  eParseError,
  eRangeError,
  eReflectionError,
  eResolveError,
  eRPCError,
  eRuntimeError,
  eTypeError,
  eUsageError,
  eVersionMismatchError,
  eWouldBlockError
};

class Status {
public:
  static Status success();

  Status(kStatusType type);
  Status(kStatusType type, const std::string& message);

  bool isError() const;
  bool isSuccess() const;

  kStatusType type() const;
  const std::string& message() const;

  void raiseIfError() const;

protected:
  kStatusType type_;
  std::string message_;
};

}
#endif

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
#ifndef _STX_BASE_STATUS_H
#define _STX_BASE_STATUS_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>

enum kStatusType {
  eSuccess,
  eBufferOverflowError,
  eConcurrentModificationError,
  eDivideByZeroError,
  eFlagError,
  eForeignError,
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
  eNotFoundError,
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

  Status(const std::exception& e);
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

#endif

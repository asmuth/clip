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

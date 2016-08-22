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
#include <limits>
#include <string.h>
#include <stdio.h>
#include <eventql/util/exception.h>
#include <eventql/util/ieee754.h>

uint64_t IEEE754::toBytes(double value) {
  uint64_t bytes;

  constexpr bool ieee754 =
      std::numeric_limits<double>::is_iec559 &&
      sizeof(double) == sizeof(uint64_t);

  static_assert(
      ieee754,
      "IEEE 754 floating point conversion not yet implemented");

  if (ieee754) {
    memcpy((void *) &bytes, (void *) &value, sizeof(bytes));
  } else {
    /* not yet implemented */
  }

  return bytes;
}

double IEEE754::fromBytes(uint64_t bytes) {
  double value;

  constexpr bool ieee754 =
      std::numeric_limits<double>::is_iec559 &&
      sizeof(double) == sizeof(uint64_t);

  static_assert(
      ieee754,
      "IEEE 754 floating point conversion not yet implemented");

  if (ieee754) {
    memcpy((void *) &value, (void *) &bytes, sizeof(bytes));
  } else {
    /* not yet implemented */
  }

  return value;
}


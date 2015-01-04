/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_DURATION_H
#define _FNORD_DURATION_H
#include <ctime>
#include <inttypes.h>
#include <limits>
#include <string>
#include "fnord/base/time_constants.h"

namespace fnord {

class Duration {
public:

  /**
   * Create a new Duration
   *
   * @param microseconds the duration in microseconds
   */
  Duration(uint64_t microseconds);

  bool operator==(const DateTime& other) const;
  bool operator<(const DateTime& other) const;
  bool operator>(const DateTime& other) const;
  bool operator<=(const DateTime& other) const;
  bool operator>=(const DateTime& other) const;

  /**
   * Return the represented duration in microseconds
   */
  explicit operator uint64_t() const;

  /**
   * Return the represented duration in microseconds
   */
  explicit operator double() const;

  /**
   * Return the represented duration in microseconds
   */
  uint64_t microseconds() const;


protected:
  uint64_t micros_;
};

}
#endif

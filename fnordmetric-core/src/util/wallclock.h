/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_WALLCLOCK_H
#define _FNORDMETRIC_UTIL_WALLCLOCK_H
#include <metricd/util/datetime.h>
#include <stdlib.h>
#include <stdint.h>

namespace fnord {
namespace util {

class WallClock {
public:
  static DateTime now();
  static uint64_t getUnixMillis();
  static uint64_t unixMillis();
  static uint64_t getUnixMicros();
  static uint64_t unixMicros();
};

}
}
#endif

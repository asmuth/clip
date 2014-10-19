/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/wallclock.h>
#include <sys/time.h>

namespace fnord {
namespace util {

DateTime WallClock::now() {
  return DateTime(WallClock::getUnixMicros());
}

uint64_t WallClock::getUnixMillis() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000u + tv.tv_usec / 1000u;
}

uint64_t WallClock::getUnixMicros() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000u + tv.tv_usec;
}

}
}

/**
 * this file is part of the "fnordmetric" project
 *   copyright (c) 2014 paul asmuth, google inc.
 *
 * licensed under the mit license (see license).
 */
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "clock.h"

namespace fnordmetric {

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

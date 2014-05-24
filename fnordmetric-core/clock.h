/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CLOCK_H
#define _FNORDMETRIC_CLOCK_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

namespace fnordmetric {

class WallClock {
public:

  static uint64_t getUnixMicros() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000u) + tv.tv_usec;
  }

};

}
#endif

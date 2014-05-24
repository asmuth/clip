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

namespace fnordmetric {

class WallClock {
public:
  static uint64_t getUnixMillis();
  static uint64_t getUnixMicros();
};

}
#endif

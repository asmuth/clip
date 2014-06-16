/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_IDSEQUENCE_H
#define _FNORDMETRIC_IDSEQUENCE_H
#include <stdlib.h>
#include <stdint.h>
#include "clock.h"

namespace fnordmetric {

/**
 */
class IDSequence {
public:
  IDSequence() {}
  IDSequence(const IDSequence& copy) = delete;
  IDSequence& operator==(const IDSequence& copy) = delete;
  virtual ~IDSequence() {}
  virtual uint64_t getNext(uint64_t last) = 0;
};

class UnixMillisecondIDSequence : public IDSequence {
public:
  UnixMillisecondIDSequence() {}
  uint64_t getNext(uint64_t last) override {
    return WallClock::getUnixMillis();
  }
};

}
#endif

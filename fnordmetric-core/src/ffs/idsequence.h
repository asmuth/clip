/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

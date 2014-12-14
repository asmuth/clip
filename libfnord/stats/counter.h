/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_MOVINGAVERAGE_H
#define _FNORD_STATS_MOVINGAVERAGE_H
#include <stdlib.h>
#include <stdint.h>
#include "fnord/base/datetime.h"

namespace fnord {
namespace stats {

template <typename T>
class Counter {
public:

  Counter(const std::string& name);

  void increment(T value);
  void set(T value);

  T value() const;

protected:
  T value_;
};

}
}
#endif

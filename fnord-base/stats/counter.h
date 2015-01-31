/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_COUNTER_H
#define _FNORD_STATS_COUNTER_H
#include <stdlib.h>
#include <stdint.h>
#include "fnord-base/datetime.h"
#include "fnord-base/hash.h"
#include "fnord/stats/stat.h"

namespace fnord {
namespace stats {

template <typename ValueType>
class CounterStat : public Stat {
public:
  CounterStat();

  void incr(ValueType value);
  void decr(ValueType value);
  void set(ValueType value);

  void exportAll(const String& path, StatsSink* sink) const override;

protected:
  std::atomic<ValueType> value_;
};

template <typename ValueType>
class Counter : public StatRef {
public:
  Counter();

  void incr(ValueType value);
  void decr(ValueType value);
  void set(ValueType value);

  RefPtr<Stat> getStat() const override;

protected:
  RefPtr<CounterStat<ValueType>> stat_;
};

}
}

#include "counter_impl.h"
#endif

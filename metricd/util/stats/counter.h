/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _STX_STATS_COUNTER_H
#define _STX_STATS_COUNTER_H
#include <stdlib.h>
#include <stdint.h>
#include "fnordmetric/util/time.h"
#include "fnordmetric/util/stats/stat.h"

namespace stats {

template <typename ValueType>
class CounterStat : public Stat {
public:
  CounterStat();

  void incr(ValueType value);
  void decr(ValueType value);
  void set(ValueType value);
  ValueType get() const;

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
  ValueType get() const;

  RefPtr<Stat> getStat() const override;

protected:
  RefPtr<CounterStat<ValueType>> stat_;
};

}

#include "counter_impl.h"
#endif

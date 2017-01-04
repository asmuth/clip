/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_STATS_MULTICOUNTER_H
#define _STX_STATS_MULTICOUNTER_H
#include <stdlib.h>
#include <stdint.h>
#include "metricd/util/time.h"
#include "metricd/util/hash.h"
#include "metricd/util/stats/stat.h"

namespace stats {

template <typename ValueType, typename... LabelTypes>
class MultiCounterStat : public stats::Stat {
public:
  typedef std::tuple<LabelTypes...> LabelValuesType;

  template <typename... LabelNameTypes>
  MultiCounterStat(LabelNameTypes... label_names);

  void increment(ValueType value);
  void set(ValueType value);

  void exportAll(const String& path, StatsSink* sink) const override;

protected:
  std::unordered_map<LabelValuesType, ValueType, fnordmetric::hash<LabelValuesType>> values_;
  mutable std::mutex mutex_;
};

template <typename ValueType, typename... LabelTypes>
class MultiCounter : public StatRef {
public:

  template <typename... LabelNameTypes>
  MultiCounter(LabelNameTypes... label_names);

  void increment(ValueType value);
  void set(ValueType value);

  RefPtr<Stat> getStat() const override;

protected:
  RefPtr<MultiCounterStat<ValueType, LabelTypes...>> stat_;
};

}

#include "multicounter_impl.h"
#endif

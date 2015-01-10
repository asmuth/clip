/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_COUNTER_IMPL_H
#define _FNORD_STATS_COUNTER_IMPL_H

namespace fnord {
namespace stats {

template <typename ValueType, typename... LabelTypes>
template <typename... LabelNameTypes>
CounterStat<ValueType, LabelTypes...>::CounterStat(
    LabelNameTypes... label_names) {
  static_assert(
      sizeof...(LabelTypes) == sizeof...(LabelNameTypes),
      "number labels names does not match number of label template types");
}

template <typename ValueType, typename... LabelTypes>
void CounterStat<ValueType, LabelTypes...>::exportAll() const {
  ScopedLock<std::mutex> lk(mutex_);
}

template <typename ValueType, typename... LabelTypes>
template <typename... LabelNameTypes>
Counter<ValueType, LabelTypes...>::Counter(
    LabelNameTypes... label_names) :
    stat_(new CounterStat<ValueType, LabelTypes...>(label_names...)) {}

template <typename ValueType, typename... LabelTypes>
RefPtr<Stat> Counter<ValueType, LabelTypes...>::getStat() const {
  return stat_;
}

/*
  void increment(ValueType value);
  void set(ValueType value);
*/

}
}
#endif

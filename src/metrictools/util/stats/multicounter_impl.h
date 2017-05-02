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
#ifndef _STX_STATS_MULTICOUNTER_IMPL_H
#define _STX_STATS_MULTICOUNTER_IMPL_H

namespace stats {

template <typename ValueType, typename... LabelTypes>
template <typename... LabelNameTypes>
MultiCounterStat<ValueType, LabelTypes...>::MultiCounterStat(
    LabelNameTypes... label_names) {
  static_assert(
      sizeof...(LabelTypes) == sizeof...(LabelNameTypes),
      "number labels names does not match number of label template types");
}

template <typename ValueType, typename... LabelTypes>
void MultiCounterStat<ValueType, LabelTypes...>::exportAll(
    const String& path,
    StatsSink* sink) const {
  ScopedLock<std::mutex> lk(mutex_);

  for (const auto& inst : values_) {
    StatsSink::Labels labels;
    sink->addStatValue(path, labels, inst.second);
  }
}

template <typename ValueType, typename... LabelTypes>
template <typename... LabelNameTypes>
MultiCounter<ValueType, LabelTypes...>::MultiCounter(
    LabelNameTypes... label_names) :
    stat_(new MultiCounterStat<ValueType, LabelTypes...>(label_names...)) {}

template <typename ValueType, typename... LabelTypes>
RefPtr<Stat> MultiCounter<ValueType, LabelTypes...>::getStat() const {
  return RefPtr<Stat>((Stat*) stat_.get());
}

/*
  void increment(ValueType value);
  void set(ValueType value);
*/

}
#endif

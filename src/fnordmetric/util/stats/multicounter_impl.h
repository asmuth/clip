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

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
#ifndef _STX_STATS_COUNTER_IMPL_H
#define _STX_STATS_COUNTER_IMPL_H

namespace stats {

template <typename ValueType>
CounterStat<ValueType>::CounterStat() : value_(0) {}

template <typename ValueType>
void CounterStat<ValueType>::exportAll(
    const String& path,
    StatsSink* sink) const {
  sink->addStatValue(path, value_.load());
}

template <typename ValueType>
void CounterStat<ValueType>::incr(ValueType value) {
  value_ += value;
}

template <typename ValueType>
void CounterStat<ValueType>::decr(ValueType value) {
  value_ -= value;
}

template <typename ValueType>
void CounterStat<ValueType>::set(ValueType value) {
  value_ = value;
}

template <typename ValueType>
ValueType CounterStat<ValueType>::get() const {
  return value_;
}

template <typename ValueType>
Counter<ValueType>::Counter() :
    stat_(new CounterStat<ValueType>()) {}

template <typename ValueType>
RefPtr<Stat> Counter<ValueType>::getStat() const {
  return RefPtr<Stat>(stat_.get());
}

template <typename ValueType>
void Counter<ValueType>::incr(ValueType value) {
  stat_->incr(value);
}

template <typename ValueType>
void Counter<ValueType>::decr(ValueType value) {
  stat_->decr(value);
}

template <typename ValueType>
void Counter<ValueType>::set(ValueType value) {
  stat_->set(value);
}

template <typename ValueType>
ValueType Counter<ValueType>::get() const {
  return stat_->get();
}

}
#endif

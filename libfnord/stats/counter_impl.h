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
Counter<ValueType>::Counter() :
    stat_(new CounterStat<ValueType>()) {}

template <typename ValueType>
RefPtr<Stat> Counter<ValueType>::getStat() const {
  return stat_;
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

}
}
#endif

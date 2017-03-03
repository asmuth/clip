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

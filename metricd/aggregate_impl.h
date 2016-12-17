/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <type_traits>

namespace fnordmetric {

template <typename T>
SumOutputAggregator<T>::SumOutputAggregator() : twin_(0), sum_(0) {}

template <typename T>
bool SumOutputAggregator<T>::aggregateUINT64(
    uint64_t input_time,
    uint64_t input_value,
    uint64_t* output_time,
    uint64_t* output_value) {
  throw std::invalid_argument("type mismatch");
}

template <typename T>
bool SumOutputAggregator<T>::aggregate(
    uint64_t input_time,
    T input_value,
    uint64_t* output_time,
    T* output_value) {
  if (input_time == uint64_t(-1)) {
    return false;
  } else {
    *output_time = input_time;
    *output_value = input_value;
    return true;
  }
}

} // namespace fnordmetric


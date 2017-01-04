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
#ifndef _STX_STATS_MOVINGAVERAGE_H
#define _STX_STATS_MOVINGAVERAGE_H
#include <stdlib.h>
#include <stdint.h>
#include "metricd/util/UnixTime.h"

namespace stats {

template <typename T>
class RingBuffer {
public:
  typedef std::function<T(T)> AggregationFunc;

  static const AggregationFunc kMeanAggregation;
  static const AggregationFunc kSumAggregation;

  RingBuffer(
      const std::string& name,
      size_t buffer_size,
      AggregationFunc aggregation);

  void insert(T value);

protected:
  T* rbuf;
  size_t rbuf_len_;
  size_t rbuf_pos_;
  size_t rbuf_used_;
};

}
#endif

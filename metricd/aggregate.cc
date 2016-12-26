/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/aggregate.h"
#include "metricd/types.h"
#include <limits>
#include <assert.h>
#include <string.h>

namespace fnordmetric {

SumInputAggregator::SumInputAggregator(
    uint64_t granularity,
    uint64_t align /* = 0 */) :
    granularity_(granularity),
    align_(align) {}

ReturnCode SumInputAggregator::addSample(
    tsdb::Cursor* cursor,
    uint64_t time,
    MetricDataType value_type,
    const void* value,
    size_t value_len) {
  auto twin = alignTime(time, granularity_, align_);
  size_t value_type_len = getMetricDataTypeSize(value_type);
  assert(value_type_len == value_len);

  /* update or insert value */
  if (cursor->seekTo(twin)) {
    if (cursor->getTime() == twin) {
      void* sum = alloca(value_type_len);
      cursor->getValue(sum, value_type_len);
      tval_add(value_type, sum, value_type_len, value, value_len);
      cursor->update(sum, value_type_len);
    } else {
      cursor->insert(twin, value, value_len);
    }
  } else {
    cursor->append(twin, value, value_len);
  }

  /* ensure that next time window entry exists */
  auto twin_next = twin + granularity_;
  void* zero = alloca(value_type_len);
  tval_zero(value_type, zero, value_type_len);
  if (cursor->next() && cursor->valid()) {
    if (cursor->getTime() > twin_next) {
      cursor->insert(twin_next, zero, value_type_len);
    }
  } else {
    cursor->append(twin_next, zero, value_type_len);
  }

  return ReturnCode::success();
}

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align = 0) {
  if (window >= std::numeric_limits<uint64_t>::max() / 2 ||
      timestamp >= std::numeric_limits<uint64_t>::max() / 2) {
    return -1;
  }

  int64_t timestamp_base = int64_t(timestamp) - int64_t(align);
  return (timestamp_base / int64_t(window)) * int64_t(window) + int64_t(align);
}

SumOutputAggregator::SumOutputAggregator(
    tsdb::Cursor* cursor,
    uint64_t granularity,
    uint64_t align /* = 0 */) :
    cursor_(cursor),
    granularity_(granularity),
    align_(align) {
  cursor_->seekToFirst();
  cursor_->get(&cur_time_, &cur_sum_, sizeof(cur_sum_));
  cur_time_ = alignTime(cur_time_, granularity_, align_);
  cursor_->next();
}

bool SumOutputAggregator::next(
    uint64_t* time,
    void* value,
    size_t value_len) {
  if (cursor_->valid()) {
    if (cursor_->getTime() <= cur_time_ + granularity_) {
      *time = cur_time_;
      assert(value_len == sizeof(cur_sum_));
      memcpy(value, &cur_sum_, sizeof(cur_sum_));
      cursor_->getValue(&cur_sum_, sizeof(cur_sum_));
      cursor_->next();
      cur_time_ += granularity_;
    } else {
      *time = cur_time_;
      assert(value_len == sizeof(cur_sum_));
      memcpy(value, &cur_sum_, sizeof(cur_sum_));
      cur_sum_ = 0;
      cur_time_ += granularity_;
    }

    return true;
  } else if (cur_time_ < uint64_t(-1)) {
    *time = cur_time_;
    assert(value_len == sizeof(cur_sum_));
    memcpy(value, &cur_sum_, sizeof(cur_sum_));
    cur_sum_ = 0;
    cur_time_ = uint64_t(-1);
    return true;
  } else {
    return false;
  }
}

} // namespace fnordmetric


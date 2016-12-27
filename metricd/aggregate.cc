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

SumOutputAggregator::SumOutputAggregator(
    tsdb::Cursor* cursor,
    MetricDataType input_type,
    uint64_t granularity,
    uint64_t align /* = 0 */,
    bool interpolate /* = true */) :
    cursor_(cursor),
    input_type_(input_type),
    granularity_(granularity),
    align_(align),
    interpolate_(interpolate) {
  cur_sum_.type = input_type_;
  cur_sum_.len = getMetricDataTypeSize(input_type_);
  cur_sum_.data = malloc(cur_sum_.len);

  cursor_->get(&cur_time_, cur_sum_.data, cur_sum_.len);
  cur_time_ = alignTime(cur_time_, granularity_, align_);
  cursor_->next();
}

SumOutputAggregator::~SumOutputAggregator() {
  free(cur_sum_.data);
}

bool SumOutputAggregator::next(
    uint64_t* time,
    tval_ref* out,
    size_t out_len) {
  if (!cursor_->valid()) {
    return false;
  }

  tval_ref val;
  val.type = input_type_;
  val.len = getMetricDataTypeSize(input_type_);
  val.data = alloca(val.len);
  while (cursor_->valid() && cursor_->getTime() < cur_time_ + granularity_) {
    cursor_->getValue(val.data, val.len);
    tval_add(cur_sum_.type, cur_sum_.data, cur_sum_.len, val.data, val.len);
    cursor_->next();
  }

  uint64_t interpolate_windows = 1;
  if (interpolate_ && cursor_->valid()) {
    auto next_time = alignTime(cursor_->getTime(), granularity_, align_);
    interpolate_windows = (next_time - cur_time_) / granularity_;
  }

  if (interpolate_windows > 1) {
    tval_ref next_val;
    next_val.type = input_type_;
    next_val.len = getMetricDataTypeSize(input_type_);
    next_val.data = alloca(val.len);

    switch (input_type_) {
      case MetricDataType::UINT64:
        *((uint64_t*) next_val.data) =
            (*((uint64_t*) cur_sum_.data) / interpolate_windows) +
            (*((uint64_t*) cur_sum_.data) % interpolate_windows);

    }

    if (out_len > 0) {
      assert(out[0].type == next_val.type);
      assert(out[0].len == next_val.len);
      memcpy(out[0].data, next_val.data, std::min(next_val.len, out[0].len));
    }

    tval_sub(
        cur_sum_.type,
        cur_sum_.data,
        cur_sum_.len,
        next_val.data,
        next_val.len);
  } else {
    if (out_len > 0) {
      assert(out[0].type == cur_sum_.type);
      assert(out[0].len == cur_sum_.len);
      memcpy(out[0].data, cur_sum_.data, std::min(cur_sum_.len, out[0].len));
    }

    tval_zero(cur_sum_.type, cur_sum_.data, cur_sum_.len);
  }

  *time = cur_time_;
  cur_time_ += granularity_;

  return true;
}

MetricDataType SumOutputAggregator::getOutputType() const {
  return MetricDataType::UINT64;
}

size_t SumOutputAggregator::getOutputColumnCount() const {
  return 1;
}

std::string SumOutputAggregator::getOutputColumnName(size_t idx) const {
  assert(idx < 1);
  return "value";
}

MaxInputAggregator::MaxInputAggregator(
    uint64_t granularity,
    uint64_t align /* = 0 */) :
    granularity_(granularity),
    align_(align) {}

ReturnCode MaxInputAggregator::addSample(
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
      void* current = alloca(value_type_len);
      cursor->getValue(current, value_type_len);
      if (tval_cmp(value_type, current, value_type_len, value, value_len) < 0) {
        cursor->update(value, value_len);
      }
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

MaxOutputAggregator::MaxOutputAggregator(
    tsdb::Cursor* cursor,
    uint64_t granularity,
    uint64_t align /* = 0 */,
    bool interpolate /* = true */) :
    cursor_(cursor),
    granularity_(granularity),
    align_(align),
    interpolate_(interpolate) {
  cursor_->get(&cur_time_, &cur_max_, sizeof(cur_max_));
  has_cur_max_ = true;
  cur_time_ = alignTime(cur_time_, granularity_, align_);
  cursor_->next();
}

bool MaxOutputAggregator::next(
    uint64_t* time,
    tval_ref* out,
    size_t out_len) {
  if (!cursor_->valid()) {
    return false;
  }

  while (cursor_->valid() && cursor_->getTime() < cur_time_ + granularity_) {
    uint64_t val;
    cursor_->getValue(&val, sizeof(val));
    if (!has_cur_max_ || val > cur_max_) {
      cur_max_ = val;
      has_cur_max_ = true;
    }
    cursor_->next();
  }

  if (out_len > 0) {
    assert(out[0].len == sizeof(cur_max_));
    memcpy(out[0].data, &cur_max_, sizeof(cur_max_));
  }

  *time = cur_time_;
  cur_time_ += granularity_;

  bool should_interpolate =
      interpolate_ &&
      cursor_->valid() &&
      cursor_->getTime() >= cur_time_ + granularity_;

  if (!should_interpolate) {
    cur_max_ = 0;
    has_cur_max_ = false;
  }

  return true;
}

MetricDataType MaxOutputAggregator::getOutputType() const {
  return MetricDataType::UINT64;
}

size_t MaxOutputAggregator::getOutputColumnCount() const {
  return 1;
}

std::string MaxOutputAggregator::getOutputColumnName(size_t idx) const {
  assert(idx < 1);
  return "value";
}

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align = 0) {
  if (window >= std::numeric_limits<uint64_t>::max() / 2 ||
      timestamp >= std::numeric_limits<uint64_t>::max() / 2) {
    return -1;
  }

  int64_t timestamp_base = int64_t(timestamp) - int64_t(align);
  return (timestamp_base / int64_t(window)) * int64_t(window) + int64_t(align);
}

} // namespace fnordmetric


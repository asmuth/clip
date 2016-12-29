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
#include "metricd/metric_cursor.h"
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
    tval_type value_type,
    const void* value,
    size_t value_len) {
  auto twin = alignTime(time, granularity_, align_);
  size_t value_type_len = tval_len(value_type);
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
    tsdb::Cursor cursor,
    tval_type input_type,
    const MetricCursorOptions* opts) :
    cursor_(std::move(cursor)),
    input_type_(input_type),
    opts_(opts) {
  cur_sum_.type = input_type_;
  cur_sum_.len = tval_len(input_type_);
  cur_sum_.data = malloc(cur_sum_.len);
  tval_zero(cur_sum_.type, cur_sum_.data, cur_sum_.len);

  cur_time_ = alignTime(opts->time_begin, opts->granularity, opts->align);
  cursor_.seekTo(cur_time_);
}

SumOutputAggregator::~SumOutputAggregator() {
  free(cur_sum_.data);
}

bool SumOutputAggregator::next(
    uint64_t* time,
    tval_ref* out,
    size_t out_len) {
  auto granularity = opts_->granularity;
  if (cur_time_ >= opts_->time_limit + granularity) {
    return false;
  }

  tval_ref val;
  val.type = input_type_;
  val.len = tval_len(input_type_);
  val.data = alloca(val.len);
  while (cursor_.valid() && cursor_.getTime() < cur_time_ + granularity) {
    cursor_.getValue(val.data, val.len);
    tval_add(cur_sum_.type, cur_sum_.data, cur_sum_.len, val.data, val.len);
    cursor_.next();
  }

  uint64_t interpolate_windows = 1;
  if (opts_->interpolate && cursor_.valid()) {
    auto next_time = alignTime(cursor_.getTime(), granularity, opts_->align);
    interpolate_windows = (next_time - cur_time_) / granularity;
  }

  if (interpolate_windows > 1) {
    tval_ref next_val;
    next_val.type = input_type_;
    next_val.len = tval_len(input_type_);
    next_val.data = alloca(val.len);

    switch (input_type_) {
      case tval_type::UINT64:
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
  cur_time_ += granularity;

  return true;
}

tval_type SumOutputAggregator::getOutputType() const {
  return input_type_;
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
    tval_type value_type,
    const void* value,
    size_t value_len) {
  auto twin = alignTime(time, granularity_, align_);
  size_t value_type_len = tval_len(value_type);
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
    tsdb::Cursor cursor,
    tval_type input_type,
    const MetricCursorOptions* opts) :
    cursor_(std::move(cursor)),
    input_type_(input_type),
    opts_(opts) {
  cur_max_.type = input_type_;
  cur_max_.len = tval_len(input_type_);
  cur_max_.data = malloc(cur_max_.len);
  tval_zero(cur_max_.type, cur_max_.data, cur_max_.len);
  has_cur_max_ = false;

  cur_time_ = alignTime(opts->time_begin, opts->granularity, opts->align);
  cursor_.seekTo(cur_time_);
}

MaxOutputAggregator::~MaxOutputAggregator() {
  free(cur_max_.data);
}

bool MaxOutputAggregator::next(
    uint64_t* time,
    tval_ref* out,
    size_t out_len) {
  auto granularity = opts_->granularity;
  if (cur_time_ >= opts_->time_limit + granularity) {
    return false;
  }

  while (cursor_.valid() && cursor_.getTime() < cur_time_ + granularity) {
    tval_ref val;
    val.type = input_type_;
    val.len = tval_len(input_type_);
    val.data = alloca(val.len);

    cursor_.getValue(val.data, val.len);
    if (!has_cur_max_ ||
        tval_cmp(val.type, val.data, val.len, cur_max_.data, cur_max_.len) > 0) {
      memcpy(cur_max_.data, val.data, val.len);
      has_cur_max_ = true;
    }

    cursor_.next();
  }

  if (out_len > 0) {
    assert(out[0].len == cur_max_.len);
    memcpy(out[0].data, cur_max_.data, cur_max_.len);
  }

  *time = cur_time_;
  cur_time_ += granularity;

  bool should_interpolate =
      opts_->interpolate &&
      cursor_.valid() &&
      cursor_.getTime() >= cur_time_ + granularity;

  if (!should_interpolate) {
    tval_zero(cur_max_.type, cur_max_.data, cur_max_.len);
    has_cur_max_ = false;
  }

  return true;
}

tval_type MaxOutputAggregator::getOutputType() const {
  return input_type_;
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


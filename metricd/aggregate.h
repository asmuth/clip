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
#include <metricd/sample.h>
#include <metricd/types.h>
#include <metricd/util/return_code.h>
#include <libtsdb/cursor.h>

namespace fnordmetric {

class InputAggregator {
public:

  virtual ~InputAggregator() = default;

  virtual ReturnCode addSample(
      tsdb::Cursor* cursor,
      uint64_t time,
      MetricDataType value_type,
      const void* value,
      size_t value_len) = 0;

};

class OutputAggregator {
public:

  virtual ~OutputAggregator() = default;

  virtual bool next(
      uint64_t* time,
      void* value,
      size_t value_len) = 0;

};

class SumInputAggregator : public InputAggregator {
public:

  SumInputAggregator(
      uint64_t granularity,
      uint64_t align = 0);

  ReturnCode addSample(
      tsdb::Cursor* cursor,
      uint64_t time,
      MetricDataType value_type,
      const void* value,
      size_t value_len) override;

protected:
  uint64_t granularity_;
  uint64_t align_;
};

class SumOutputAggregator : public OutputAggregator {
public:

  SumOutputAggregator(
      tsdb::Cursor* cursor,
      uint64_t granularity,
      uint64_t align = 0,
      bool interpolate = true);

  bool next(
      uint64_t* time,
      void* value,
      size_t value_len) override;

protected:
  tsdb::Cursor* cursor_;
  uint64_t granularity_;
  uint64_t align_;
  bool interpolate_;
  uint64_t cur_time_;
  uint64_t cur_sum_;
};

uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align);

} // namespace fnordmetric


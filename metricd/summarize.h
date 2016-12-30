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
#include <metricd/types.h>
#include <metricd/util/return_code.h>
#include <metricd/query/data_frame.h>

namespace fnordmetric {

enum class GrossSummaryMethod {
  SUM,
  MAX,
  MIN,
  TREND
};

class GroupSummary {
public:

  virtual ~GroupSummary() = default;

  virtual void addValue(
      tval_type value_type,
      const void* value,
      size_t value_len) = 0;

  virtual void getValue(
      tval_type value_type,
      void* value,
      size_t value_len) = 0;

  virtual void reset() = 0;

};

std::string getGrossSummaryName(GrossSummaryMethod method);

ReturnCode summarizeTimeseries(
    GrossSummaryMethod method,
    const DataFrame* frame,
    tval_autoref* result);

class SumGroupSummary : public GroupSummary {
public:

  SumGroupSummary(tval_type val_type);

  void addValue(
      tval_type value_type,
      const void* value,
      size_t value_len) override;

  void getValue(
      tval_type value_type,
      void* value,
      size_t value_len) override;

  void reset() override;


protected:
  tval_autoref acc_;
};

} // namespace fnordmetric


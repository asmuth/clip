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
#include <fnordmetric/sample.h>
#include <fnordmetric/util/return_code.h>

namespace fnordmetric {
class MeasureConfig;

enum class AggregationFunctionType {
  SUM, MIN, MAX
};

std::string getAggregationFunctionTypeName(AggregationFunctionType t);
bool parseAggregationFunctionType(const std::string& s, AggregationFunctionType* t);

class AggregationFunction {
public:

  virtual ~AggregationFunction() = default;

  virtual ReturnCode addSample(
      const MeasureConfig& config,
      const Sample& sample) = 0;

  virtual void getResult(
      const MeasureConfig& config,
      std::vector<std::pair<std::string, std::string>>* columns) const = 0;

};

class SumAggregationFunction : public AggregationFunction {
public:

  SumAggregationFunction(const MeasureConfig& config);

  ReturnCode addSample(
      const MeasureConfig& config,
      const Sample& sample) override;

  void getResult(
      const MeasureConfig& config,
      std::vector<std::pair<std::string, std::string>>* columns) const override;

protected:
  uint64_t sum_;
};

//
//class SumOutputAggregator : public OutputAggregator {
//public:
//
//  SumOutputAggregator(
//      tval_type input_type,
//      const MetricCursorOptions* opts);
//
//  ~SumOutputAggregator();
//
//  bool next(uint64_t* time, tval_ref* out) override;
//
//protected:
//  tval_type input_type_;
//  const MetricCursorOptions* opts_;
//  uint64_t cur_time_;
//  tval_ref cur_sum_;
//  bool have_value_;
//};
//
//class MaxInputAggregator : public InputAggregator {
//public:
//
//  MaxInputAggregator(
//      uint64_t granularity,
//      uint64_t align = 0);
//
//  ReturnCode addSample(
//      uint64_t time,
//      tval_type value_type,
//      const void* value,
//      size_t value_len) override;
//
//protected:
//  uint64_t granularity_;
//  uint64_t align_;
//};
//
//class MaxOutputAggregator : public OutputAggregator {
//public:
//
//  MaxOutputAggregator(
//      tval_type input_type,
//      const MetricCursorOptions* opts);
//
//  ~MaxOutputAggregator();
//
//  bool next(uint64_t* time, tval_ref* out) override;
//
//protected:
//  tval_type input_type_;
//  const MetricCursorOptions* opts_;
//  uint64_t cur_time_;
//  tval_ref cur_max_;
//  bool has_cur_max_;
//  size_t cur_max_time_;
//};
//
//uint64_t alignTime(uint64_t timestamp, uint64_t window, uint64_t align);

} // namespace fnordmetric


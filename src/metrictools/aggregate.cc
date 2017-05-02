/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metrictools/aggregate.h"
#include "metrictools/table.h"
#include <limits>
#include <assert.h>
#include <string.h>

namespace fnordmetric {

std::string getAggregationFunctionTypeName(AggregationFunctionType t) {
  switch (t) {
    case AggregationFunctionType::SUM: return "sum";
    case AggregationFunctionType::MIN: return "min";
    case AggregationFunctionType::MAX: return "max";
  }

  return "???";
}

bool parseAggregationFunctionType(
    const std::string& s,
    AggregationFunctionType* t) {
  if (s == "sum") { *t = AggregationFunctionType::SUM; return true; }
  if (s == "min") { *t = AggregationFunctionType::MIN; return true; }
  if (s == "max") { *t = AggregationFunctionType::MAX; return true; }
  return false;
}

ReturnCode mkAggregationFunction(
    const MeasureConfig& config,
    std::unique_ptr<AggregationFunction>* fun) {
  switch (config.aggr_fun) {
    case AggregationFunctionType::SUM:
      switch (config.type) {
        case DataType::UINT64:
        case DataType::INT64:
          fun->reset(new SumIntegerAggregationFunction());
          return ReturnCode::success();
        default:
          return ReturnCode::error("ERUNTIME", "invalid data type for SUM");
      }
  }

  return ReturnCode::error("ERUNTIME", "invalid aggregation function");
}

ReturnCode getAggregationFunctionOutputColumns(
    const MeasureConfig& config,
    std::vector<std::pair<std::string, DataType>>* columns) {
  switch (config.aggr_fun) {
    case AggregationFunctionType::SUM:
      switch (config.type) {
        case DataType::UINT64:
        case DataType::INT64:
          columns->emplace_back(config.column_name, DataType::INT64);
          return ReturnCode::success();
        default:
          return ReturnCode::error("ERUNTIME", "invalid data type for SUM");
      }
  }

  return ReturnCode::error("ERUNTIME", "invalid aggregation function");
}

SumIntegerAggregationFunction::SumIntegerAggregationFunction() : sum_(0) {}

ReturnCode SumIntegerAggregationFunction::addSample(
    const MeasureConfig& config,
    const Sample& sample) {
  try {
    sum_ += std::stoll(sample.getValue());
  } catch (...) {
    return ReturnCode::errorf("ERUNTIME", "invalid value: $0", sample.getValue());
  }

  return ReturnCode::success();
}

void SumIntegerAggregationFunction::getResult(
    const MeasureConfig& config,
    std::vector<std::pair<std::string, std::string>>* columns) const {
  columns->emplace_back(config.column_name, std::to_string(sum_));
}

} // namespace fnordmetric


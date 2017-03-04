/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnordmetric/aggregate.h"
#include "fnordmetric/table.h"
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

SumAggregationFunction::SumAggregationFunction(
    const MeasureConfig& config) :
    sum_(0) {}

ReturnCode SumAggregationFunction::addSample(
    const MeasureConfig& config,
    const Sample& sample) {
  try {
    sum_ += std::stoull(sample.getValue());
  } catch (...) {
    return ReturnCode::errorf("ERUNTIME", "invalid value: $0", sample.getValue());
  }

  return ReturnCode::success();
}

void SumAggregationFunction::getResult(
    const MeasureConfig& config,
    std::vector<std::pair<std::string, std::string>>* columns) const {
  columns->emplace_back(config.column_name, std::to_string(sum_));
}

} // namespace fnordmetric


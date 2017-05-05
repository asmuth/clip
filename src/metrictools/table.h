/**
 * This file is part of the "FnordTable" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordTable is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metrictools/measure.h>
#include <metrictools/util/return_code.h>
#include <metrictools/util/option.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <mutex>

namespace fnordmetric {
class TableMap;

using TableIDType = std::string;

enum class DataType {
  NIL,
  UINT64,
  INT64,
  FLOAT64,
  STRING,
  TIMESTAMP
};

enum class AggregationFunctionType {
  SUM, MIN, MAX
};

std::string getDataTypeName(DataType t);
bool parseDataType(const std::string& s, DataType* t);

struct LabelConfig {
  LabelConfig(const std::string& n, DataType t = DataType::STRING);
  const std::string column_name;
  const DataType type;
};

struct MeasureConfig {
  MeasureConfig(const std::string& n, DataType t, AggregationFunctionType f);
  const std::string column_name;
  const DataType type;
  const AggregationFunctionType aggr_fun;
};

struct TableConfig {
  TableConfig(const std::string& id);
  const std::string table_id;
  uint64_t interval;
  std::vector<LabelConfig> labels;
  std::vector<MeasureConfig> measures;
};


} // namespace fnordmetric


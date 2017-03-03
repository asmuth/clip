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
#include <fnordmetric/types.h>
#include <fnordmetric/sample.h>
#include <fnordmetric/util/return_code.h>
#include <fnordmetric/aggregate.h>
#include <fnordmetric/util/option.h>
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
  STRING
};

struct LabelConfig {
  LabelConfig(const std::string n, DataType t);
  std::string column_name;
  DataType type;
};

struct MeasureConfig {
  MeasureConfig(const std::string n, DataType t, AggregationFunction f);
  std::string column_name;
  DataType type;
  AggregationFunctionType aggr_fun;
};

struct TableConfig {
  TableConfig();
  uint64_t interval;
  std::vector<LabelConfig> labels;
  std::vector<MeasureConfig> measures;
};


} // namespace fnordmetric


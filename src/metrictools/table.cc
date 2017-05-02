/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <sstream>
#include <metrictools/table.h>
#include <metrictools/table_map.h>
#include <metrictools/util/logging.h>
#include <metrictools/util/time.h>

namespace fnordmetric {

std::string getDataTypeName(DataType t) {
  switch (t) {
    case DataType::NIL: return "nil";
    case DataType::UINT64: return "uint64";
    case DataType::INT64: return "int64";
    case DataType::FLOAT64: return "float64";
    case DataType::STRING: return "string";
    case DataType::TIMESTAMP: return "timestamp";
  }

  return "???";
}

bool parseDataType(
    const std::string& s,
    DataType* t) {
  if (s == "nil") { *t = DataType::NIL; return true; }
  if (s == "uint64") { *t = DataType::UINT64; return true; }
  if (s == "int64") { *t = DataType::INT64; return true; }
  if (s == "float64") { *t = DataType::FLOAT64; return true; }
  if (s == "string") { *t = DataType::STRING; return true; }
  if (s == "timestamp") { *t = DataType::TIMESTAMP; return true; }
  return false;
}

TableConfig::TableConfig(
    const std::string& id) :
    table_id(id),
    interval(10 * kMicrosPerSecond) {}

LabelConfig::LabelConfig(
    const std::string& n,
    DataType t /* = DataType::STRING */) :
    column_name(n),
    type(t) {}

MeasureConfig::MeasureConfig(
    const std::string& n,
    DataType t,
    AggregationFunctionType f) :
    column_name(n),
    type(t),
    aggr_fun(f) {}

} // namespace fnordmetric


/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "data.h"
#include "utils/fileutil.h"
#include "utils/csv.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include <assert.h>
#include <iostream>

namespace fviz {

size_t series_len(const Series& s) {
  return s.size();
}

bool series_is_numeric(const Series& s) {
  for (const auto& v : s) {
    if (!StringUtil::isNumber(v)) {
      size_t idx;
      try {
        std::stod(v, &idx);
      } catch (...) {
        return false;
      }

      if (idx != v.size()) {
        return false;
      }
    }
  }

  return true;
}

std::vector<double> series_to_float(const Series& s) {
  std::vector<double> sf;

  for (const auto& v : s) {
    sf.push_back(value_to_float(v));
  }

  return sf;
}

double value_to_float(const Value& v) {
  try {
    return std::stod(v);
  } catch (...) {
    return 0;
  }
}

Value value_from_float(double v) {
  return std::to_string(v);
}

std::vector<DataGroup> series_group(const Series& data) {
  std::vector<DataGroup> groups;
  std::unordered_map<Value, size_t> group_map;

  for (size_t idx = 0; idx < data.size(); ++idx) {
    size_t group_idx = group_map[data[idx]];
    if (!group_idx) {
      DataGroup g;
      g.key = data[idx];
      groups.emplace_back(g);
      group_idx = groups.size();
      group_map[data[idx]] = group_idx;
    }

    assert(group_idx > 0);
    groups[group_idx - 1].index.push_back(idx);
  }

  return groups;
}

ReturnCode data_load_strings_csv(
    const Expr* expr,
    std::vector<std::string>* values) {
  auto args = expr_collect(expr);
  if (args.size() != 2 ||
      !expr_is_value(args[0]) ||
      !expr_is_value(args[1])) {
    return errorf(
        ERROR,
        "invalid number of arguments to 'csv'; expected: 2, got: {}",
        "..."); // FIXME
  }


  const auto& path = expr_get_value(args[0]);
  const auto& column_name = expr_get_value(args[1]);

  std::string data_str;
  if (auto rc = read_file(path, &data_str); !rc) {
    return rc;
  }

  auto data_csv = CSVData{};
  if (auto rc = csv_parse(data_str, &data_csv); !rc) {
    return rc;
  }

  if (data_csv.empty()) {
    return OK;
  }

  const auto& headers = data_csv.front();
  const auto& header = std::find(headers.begin(), headers.end(), column_name);
  if (header == headers.end()) {
    return errorf(
        ERROR,
        "CSV column not found: {}",
        column_name);
  }

  auto column_idx = std::distance(headers.begin(), header);
  for (auto row = ++data_csv.begin(); row != data_csv.end(); ++row) {
    if (row->size() < column_idx) {
      return errorf(
          ERROR,
          "CSV invalid number of columns for row #{}",
          std::distance(data_csv.begin(), row));
    }

    values->push_back(row->at(column_idx));
  }

  return OK;
}

ReturnCode data_load_csv(
    const Expr* expr,
    std::vector<Measure>* values) {
  std::vector<std::string> values_str;
  if (auto rc = data_load_strings_csv(expr, &values_str); !rc) {
    return rc;
  }

  for (auto v = values_str.begin(); v != values_str.end(); ++v) {
    try {
      values->push_back(from_user(std::stod(*v)));
    } catch (... ) {
      return errorf(
          ERROR,
          "CSV invalid column in row #{}: '{}' is not a number",
          std::distance(values_str.begin(), v) + 1,
          *v);
    }
  }

  return OK;
}

ReturnCode data_load_strings(
    const Expr* expr,
    std::vector<std::string>* values) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a value, got: {}",
        "..."); // FIXME
  }

  auto args = expr_get_list(expr);

  if (args && expr_is_value_literal(args, "csv")) {
    return data_load_strings_csv(expr_next(args), values);
  }

  return expr_to_strings(expr, values);
}

ReturnCode data_load(
    const Expr* expr,
    std::vector<Measure>* values) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a value, got: {}",
        "..."); // FIXME
  }

  auto args = expr_get_list(expr);

  if (args && expr_is_value_literal(args, "csv")) {
    return data_load_csv(expr_next(args), values);
  }

  return measure_read_list(expr, values);
}

ReturnCode data_to_measures(
    std::vector<std::string>& src,
    const ScaleConfig& scale,
    std::vector<Measure>* dst) {
  for (auto v = src.begin(); v != src.end(); ++v) {
    Measure m;
    switch (scale.kind) {
      case ScaleKind::CATEGORICAL: {
        auto v_iter = scale.categories_map.find(*v);
        if (v_iter == scale.categories_map.end()) {
          return errorf(
              ERROR,
              "error while parsing data: value '{}' is not part of the categories list",
              *v);
        }

        m = from_rel(scale_translate_categorical(scale, v_iter->second));
        break;
      }
      default:
        if (auto rc = parse_measure(*v, &m); !rc) {
          return errorf(
              ERROR,
              "error while parsing data: '{}': {} -- "
              "if this is intentional, set 'scale-[x,y] to (categorical ...)'",
              *v,
              rc.message);
        }
        break;
    }

    dst->push_back(m);
  }

  return OK;
}


} // namespace fviz


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

std::vector<Color> groups_to_colors(
    size_t count,
    const std::vector<DataGroup>& groups,
    const ColorScheme& palette) {
  std::vector<Color> colors(count);
  for (size_t gi = 0; gi < groups.size(); ++gi) {
    for (auto i : groups[gi].index) {
      if (i < colors.size()) {
        colors[i] = palette.get(gi);
      }
    }
  }

  return colors;
}

ReturnCode data_load_strings_csv(
    const Expr* expr,
    std::vector<std::string>* values) {
  auto args = expr_collect(expr);
  if (args.size() != 2 ||
      !expr_is_value(args[0]) ||
      !expr_is_value(args[1])) {
    return ReturnCode::errorf(
        "EARG",
        "invalid number of arguments to 'csv'; expected: 2, got: $0",
        "..."); // FIXME
  }


  const auto& path = expr_get_value(args[0]);
  const auto& column_name = expr_get_value(args[1]);

  std::string data_str;
  if (auto rc = read_file(path, &data_str); !rc) {
    return rc;
  }

  auto data_csv = CSVData{};
  CSVParserConfig parser_opts;
  if (auto rc = parseCSV(data_str, parser_opts, &data_csv); !rc) {
    return rc;
  }

  if (data_csv.empty()) {
    return OK;
  }

  const auto& headers = data_csv.front();
  const auto& header = std::find(headers.begin(), headers.end(), column_name);
  if (header == headers.end()) {
    return ReturnCode::errorf(
        "EARG",
        "CSV column not found: $0",
        column_name);
  }

  auto column_idx = std::distance(headers.begin(), header);
  for (auto row = ++data_csv.begin(); row != data_csv.end(); ++row) {
    if (row->size() < column_idx) {
      return ReturnCode::errorf(
          "EARG",
          "CSV invalid number of columns for row #$0",
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
      return ReturnCode::errorf(
          "EARG",
          "CSV invalid column in row #$0: '$1' is not a number",
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
    return ReturnCode::errorf(
        "EARG",
        "argument error; expected a value, got: $0",
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
    return ReturnCode::errorf(
        "EARG",
        "argument error; expected a value, got: $0",
        "..."); // FIXME
  }

  auto args = expr_get_list(expr);

  if (args && expr_is_value_literal(args, "csv")) {
    return data_load_csv(expr_next(args), values);
  }

  return expr_to_measures(expr, values);
}

} // namespace fviz


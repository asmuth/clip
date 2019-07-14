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
#include "config_helpers.h"
#include "utils/fileutil.h"
#include "utils/csv.h"
#include "utils/algo.h"
#include <iostream>

using namespace std::placeholders;

namespace fviz {

/*

ParserFn configure_alt(const ParserDefinitions& parsers) {
  return [parsers] (const Expr* prop) -> ReturnCode {
    std::string parser_key = "";
    if (plist::is_enum(prop) && parsers.count(prop.value)) {
      parser_key = prop.value;
    }

    return parsers.at(parser_key)(prop);
  };
}

ReturnCode configure_float(
    const Expr* prop,
    double* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  try {
    *value = std::stod(prop);
  } catch (... ) {
    return ERROR;
  }

  return OK;
}

ReturnCode configure_float_opt(
    const Expr* prop,
    std::optional<double>* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  try {
    *value = std::optional<double>(std::stod(prop));
  } catch (... ) {
    return ERROR;
  }

  return OK;
}
*/


/*
ReturnCode configure_direction(
    const Expr* prop,
    Direction* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  static const EnumDefinitions<Direction> defs = {
    { "horizontal", Direction::HORIZONTAL },
    { "vertical", Direction::VERTICAL },
  };

  return parseEnum(defs, prop.value, value);
}

ReturnCode configure_position(
    const Expr* prop,
    Position* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  static const EnumDefinitions<Position> defs = {
    { "relative", Position::RELATIVE },
    { "absolute", Position::ABSOLUTE },
    { "top", Position::TOP },
    { "right", Position::RIGHT },
    { "bottom", Position::BOTTOM },
    { "left", Position::LEFT },
  };

  return parseEnum(defs, prop.value, value);
}

ReturnCode load_csv(
    const std::string& csv_path,
    bool csv_headers,
    SeriesMap* data) {
  std::string csv_data_str;
  if (auto rc = read_file(csv_path, &csv_data_str); !rc) {
    return rc;
  }

  auto csv_data = CSVData{};
  CSVParserConfig csv_opts;
  if (auto rc = parseCSV(csv_data_str, csv_opts, &csv_data); !rc) {
    return rc;
  }

  std::optional<size_t> column_count;
  for (const auto& row : csv_data) {
    if (!column_count || row.size() < column_count) {
      column_count = row.size();
    }
  }

  for (size_t i = 0; i < column_count; ++i) {
    std::string series_name;

    auto row = csv_data.begin();
    if (csv_headers &&
        row != csv_data.end() &&
        row->size() > i) {
      series_name = (row++)->at(i);
    } else {
      series_name = std::to_string(i);
    }

    auto series = std::make_shared<Series>();
    for (; row != csv_data.end(); ++row) {
      if (row->size() > i) {
        series->emplace_back(row->at(i));
      } else {
        series->emplace_back();
      }
    }

    (*data)[series_name] = series;
  }

  return OK;
}

ReturnCode parse_data_series_csv(
    const Expr* prop,
    SeriesRef* data_ref) {
  if (!plist::is_enum(prop, "csv")) {
    return ERROR;
  }

  if (prop.size() < 2) {
    return ReturnCode::errorf("EARG", "csv() takes exactly two or more arguments, got: $0", prop.size());
  }

  const auto& csv_path = prop[0].value;
  const auto& csv_column = prop[1].value;
  bool csv_headers = true;

  for (size_t i = 2; i < prop.size(); ++i) {
    if (prop[i].value == "noheaders") {
      csv_headers = false;
      continue;
    }
  }

  SeriesMap csv_data;
  if (auto rc = load_csv(csv_path, csv_headers, &csv_data); !rc) {
    return rc;
  }

  *data_ref = find_maybe(csv_data, csv_column);

  if (*data_ref) {
    return OK;
  } else {
    return ReturnCode::errorf(
        "EARG",
        "CSV file '$0' has no column named '$1'", 
        csv_path,
        csv_column);
  }
}

*/
} // namespace fviz


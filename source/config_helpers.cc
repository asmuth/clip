/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config_helpers.h"
#include "utils/fileutil.h"
#include "utils/csv.h"
#include "utils/algo.h"
#include <iostream>

using namespace std::placeholders;

namespace plotfx {

ReturnCode parseAll(
    const plist::PropertyList& plist,
    const ParserDefinitions& pdefs) {
  for (const auto& prop : plist) {
    const auto& pdef = pdefs.find(prop.name);
    if (pdef != pdefs.end()) {
      if (auto rc = pdef->second(prop); !rc.isSuccess()) {
        return ReturnCode::errorf(
            "EPARSE",
            "error while parsing property '$0': $1",
            prop.name,
            rc.getMessage());

        return rc;
      }
    }
  }

  return ReturnCode::success();
}

ReturnCode configure_measure(
    const plist::Property& prop,
    Measure* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parse_measure(prop, value);
}

ReturnCode configure_measure_opt(
    const plist::Property& prop,
    std::optional<Measure>* value) {
  Measure v;
  if (auto rc = configure_measure(prop, &v); !rc) {
    return rc;
  }

  *value = v;
  return OK;
}

ParserFn configure_multiprop(const std::vector<ParserFn>& parsers) {
  return [parsers] (const plist::Property& prop) -> ReturnCode {
    for (const auto& p : parsers) {
      if (auto rc = p(prop); !rc) {
        return rc;
      }
    }

    return OK;
  };
}

ParserFn configure_alt(const ParserDefinitions& parsers) {
  return [parsers] (const plist::Property& prop) -> ReturnCode {
    std::string parser_key = "";
    if (plist::is_enum(prop) && parsers.count(prop.value)) {
      parser_key = prop.value;
    }

    return parsers.at(parser_key)(prop);
  };
}

ReturnCode configure_color(
    const plist::Property& prop,
    Color* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  if (StringUtil::beginsWith(prop, "#")) {
    if (value->parse(prop)) {
      return OK;
    }
  }

  return ReturnCode::error("EARG", "invalid color");
}

ParserFn configure_color_opt(std::optional<Color>* var) {
  return [=] (const plist::Property& prop) -> ReturnCode {
    Color c;
    if (auto rc = configure_color(prop, &c); !rc) {
      return rc;
    }

    *var = c;
    return OK;
  };
}

ParserFn configure_color_fn(Color* var) {
  return bind(&configure_color, _1, var);
}

ReturnCode configure_float(
    const plist::Property& prop,
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
    const plist::Property& prop,
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

ReturnCode configure_string(
    const plist::Property& prop,
    std::string* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  *value = prop;
  return OK;
}

ReturnCode configure_direction(
    const plist::Property& prop,
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
    const plist::Property& prop,
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
  auto csv_data_str = FileUtil::read(csv_path).toString();
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
    const plist::Property& prop,
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

ReturnCode configure_strings(
    const plist::Property& prop,
    std::vector<std::string>* data) {
  if (plist::is_enum(prop, "csv")) {
    SeriesRef d;
    if (auto rc = parse_data_series_csv(prop, &d); !rc) {
      return rc;
    }

    for (auto v : *d) {
      data->emplace_back(v);
    }

    return OK;
  }

  return configure_vec<std::string>(bind(&configure_string, _1, _2), data)(prop);
}

ReturnCode configure_measures(
    const plist::Property& prop,
    std::vector<Measure>* measures) {
  if (plist::is_enum(prop, "csv")) {
    SeriesRef data;
    if (auto rc = parse_data_series_csv(prop, &data); !rc) {
      return rc;
    }

    for (auto v : series_to_float(*data)) {
      measures->emplace_back(from_user(v));
    }

    return OK;
  }

  return configure_vec<Measure>(bind(&configure_measure, _1, _2), measures)(prop);
}

} // namespace plotfx


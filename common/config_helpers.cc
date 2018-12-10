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
#include <iostream>

namespace plotfx {

ReturnCode configure_measure_rel(
    const plist::Property& prop,
    double dpi,
    double font_size,
    Measure* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parse_measure_rel(prop, dpi, font_size, value);
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
    return ERROR_INVALID_ARGUMENT;
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
    return ERROR_INVALID_ARGUMENT;
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

ReturnCode parse_data_frame_csv(
    const plist::Property& prop ,
    DataFrame* data) {
  if (!plist::is_enum(prop, "csv")) {
    return ERROR_INVALID_ARGUMENT;
  }

  if (prop.size() < 1) {
    return ERROR_INVALID_ARGUMENT; // FIXME
  }

  const auto& csv_path = prop[0].value;
  auto csv_data_str = FileUtil::read(csv_path).toString();
  auto csv_data = CSVData{};
  auto csv_opts = CSVParserConfig{};

  for (size_t i = 1; i < prop.size(); ++i) {
    if (prop[i].value == "noheaders") {
      csv_opts.headers = false;
      continue;
    }
  }

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
    DataColumn column;
    column.name = std::to_string(i);
    for (const auto& row : csv_data) {
      column.data.push_back(row[i]);
    }

    data->columns.emplace_back(column);
  }

  return OK;
}

ReturnCode configure_data_frame(
    const plist::Property& prop,
    DataFrame* data) {
  if (plist::is_enum(prop, "csv")) {
    return parse_data_frame_csv(prop, data);
  }

  return ERROR_INVALID_ARGUMENT;
}

} // namespace plotfx


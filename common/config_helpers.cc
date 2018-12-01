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


ReturnCode parse_classlike(
    const plist::Property& prop,
    const std::string& fn,
    std::vector<std::string>* args) {
  if (prop.size() < 3) {
    return ERROR_INVALID_ARGUMENT;
  }

  if (!prop[0].is_literal || prop[0].data != fn) {
    return ERROR_INVALID_ARGUMENT;
  }

  if (!prop[1].is_literal || prop[1].data != "(") {
    return ERROR_INVALID_ARGUMENT;
  }

  size_t argidx = 2;
  for (; argidx < prop.size() - 1; ++argidx) {
    if (prop[argidx].is_literal && prop[argidx].data == ",") {
      continue;
    }

    if (prop[argidx].is_literal && prop[argidx].data == ")") {
      break;
    }

    args->emplace_back(prop[argidx].data);
    continue;
  }

  if (argidx >= prop.size()) {
    return ERROR_INVALID_ARGUMENT;
  }

  if (!prop[argidx].is_literal || prop[argidx].data != ")") {
    return ERROR_INVALID_ARGUMENT;
  }

  return OK;
}

ReturnCode parse_data_series_csv(
    const plist::Property& prop ,
    std::vector<double>* data) {
  std::vector<std::string> args;
  if (auto rc = parse_classlike(prop, "csv", &args); !rc) {
    return rc;
  }

  if (args.size() < 2) {
    return ERROR_INVALID_ARGUMENT; // FIXME
  }

  const auto& csv_path = args[0];
  const auto& csv_column = args[1];

  size_t csv_column_idx = 0;
  try {
    csv_column_idx = std::stoul(csv_column);
  } catch (... ) {
    return ERROR_INVALID_ARGUMENT; // FIXME
  }

  auto csv_data_str = FileUtil::read(csv_path).toString();
  auto csv_data = CSVData{};
  auto csv_opts = CSVParserConfig{};

  for (size_t i = 2; i < args.size(); ++i) {
    if (args[i] == "noheaders") {
      csv_opts.headers = false;
      continue;
    }
  }

  if (auto rc = parseCSV(csv_data_str, csv_opts, &csv_data); !rc) {
    return rc;
  }

  for (const auto& row : csv_data) {
    if (row.size() > csv_column_idx) {
      double value;
      try {
        value = std::stod(row[csv_column_idx]);
      } catch (... ) {
        return ERROR_INVALID_ARGUMENT; // FIXME
      }

      data->emplace_back(value);
    }
  }

  return OK;
}

ReturnCode parse_data_series_inline(
    const plist::Property& prop ,
    std::vector<double>* data) {
  for (const auto& v : prop.values) {
    double value;
    try {
      value = std::stod(v.data);
    } catch (... ) {
      return ERROR_INVALID_ARGUMENT;
    }

    data->emplace_back(value);
  }

  return OK;
}

ReturnCode parseDataSeries(
    const plist::Property& prop,
    std::vector<double>* data) {
  if (prop.size() > 0 && prop[0].is_literal && prop[0].data == "csv") {
    return parse_data_series_csv(prop, data);
  }

  return parse_data_series_inline(prop, data);
}

ReturnCode parseMeasureProp(
    const plist::Property& prop,
    Measure* value) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parse_measure(prop[0], value);
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

ReturnCode configure_colour(
    const plist::Property& prop,
    Colour* value) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  if (prop.size() > 0 && StringUtil::beginsWith(prop[0].data, "#")) {
    if (value->parse(prop[0].data)) {
      return OK;
    }
  }

  return ReturnCode::error("EARG", "invalid colour");
}

ReturnCode configure_float(
    const plist::Property& prop,
    double* value) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  try {
    *value = std::stod(prop[0].data);
  } catch (... ) {
    return ERROR_INVALID_ARGUMENT;
  }

  return OK;
}

ReturnCode configure_float_opt(
    const plist::Property& prop,
    std::optional<double>* value) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  try {
    *value = std::optional<double>(std::stod(prop[0].data));
  } catch (... ) {
    return ERROR_INVALID_ARGUMENT;
  }

  return OK;
}

} // namespace plotfx


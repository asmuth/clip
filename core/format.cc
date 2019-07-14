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
#include "format.h"
#include "data_model.h"
#include "utils/UnixTime.h"

#include <sstream>
#include <iomanip>

namespace fviz {

Formatter format_decimal_scientific(size_t precision) {
  return [precision] (const std::string& v) -> std::string {
    std::stringstream s;
    s << std::scientific << std::setprecision(precision) << value_to_float(v);
    return s.str();
  };
}

/*
ReturnCode confgure_format_decimal_scientific(
    const plist::Property& prop,
    Formatter* formatter) {
  uint32_t precision = 1;
  switch (prop.size()) {
    case 0:
      break;
    case 1:
      try {
        precision = std::stod(prop[0]);
        break;
      } catch (... ) {
        return ERROR;
      }
    default:
      return ERROR;
  }

  *formatter = format_decimal_scientific(precision);
  return OK;
}
*/

Formatter format_decimal_fixed(size_t precision) {
  return [precision] (const std::string& v) -> std::string {
    std::stringstream s;
    s << std::fixed << std::setprecision(precision) << value_to_float(v);
    return s.str();
  };
}

/*
ReturnCode confgure_format_decimal_fixed(
    const plist::Property& prop,
    Formatter* formatter) {
  if (!plist::is_enum(prop, "fixed")) {
    return ERROR;
  }

  uint32_t precision = 1;
  switch (prop.size()) {
    case 0:
      break;
    case 1:
      try {
        precision = std::stod(prop[0]);
        break;
      } catch (... ) {
        return ERROR;
      }
    default:
      return ERROR;
  }

  *formatter = format_decimal_fixed(precision);
  return OK;
}
*/

Formatter format_datetime(const std::string& fmt) {
  return [fmt] (const std::string& v) -> std::string {
    UnixTime v_t(value_to_float(v) * 1000000);
    return v_t.toString(fmt.c_str());
  };
}

/*
ReturnCode confgure_format_datetime(
    const plist::Property& prop,
    Formatter* formatter) {
  if (!plist::is_enum(prop, "datetime")) {
    return ERROR;
  }

  std::string fmtspec = "%Y-%m-%d %H:%M:%S"; // FIXME improved auto format
  switch (prop.size()) {
    case 0:
      break;
    case 1:
      fmtspec = prop[0];
      break;
    default:
      return ERROR;
  }

  *formatter = format_datetime(fmtspec);
  return OK;
}
*/

Formatter format_string() {
  return [] (const std::string& v) -> std::string {
    return v;
  };
}

/*
ReturnCode confgure_format_string(
    const plist::Property& prop,
    Formatter* formatter) {
  *formatter = format_string();
  return OK;
}

ReturnCode confgure_format(
    const plist::Property& prop,
    Formatter* formatter) {
  if (plist::is_value(prop, "fixed") ||
      plist::is_enum(prop, "fixed")) {
    return confgure_format_decimal_fixed(prop, formatter);
  }

  if (plist::is_value(prop, "scientific") ||
      plist::is_enum(prop, "scientific")) {
    return confgure_format_decimal_scientific(prop, formatter);
  }

  if (plist::is_value(prop, "datetime") ||
      plist::is_enum(prop, "datetime")) {
    return confgure_format_datetime(prop, formatter);
  }

  if (plist::is_value(prop, "string") ||
      plist::is_enum(prop, "string")) {
    return confgure_format_string(prop, formatter);
  }

  return ReturnCode::errorf(
      "EARG",
      "invalid value '$0', expected one of: \n"
      "  - fixed\n"
      "  - scientific\n",
      "  - datetime\n",
      "  - string\n",
      prop.value);
}
*/

} // namespace fviz


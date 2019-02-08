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
#include <sstream>
#include <iomanip>
#include "core/format.h"
#include "source/config_helpers.h"
#include "source/utils/UnixTime.h"

namespace plotfx {

Formatter format_decimal_scientific(size_t precision) {
  return [precision] (const Value& v) -> std::string {
    std::stringstream s;
    s << std::scientific << std::setprecision(precision) << value_to_float(v);
    return s.str();
  };
}

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

Formatter format_decimal_fixed(size_t precision) {
  return [precision] (const Value& v) -> std::string {
    std::stringstream s;
    s << std::fixed << std::setprecision(precision) << value_to_float(v);
    return s.str();
  };
}

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

Formatter format_datetime(const std::string& fmt) {
  return [fmt] (const Value& v) -> std::string {
    UnixTime v_t(value_to_float(v) * 1000000);
    return v_t.toString(fmt.c_str());
  };
}

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

Formatter format_string() {
  return [] (const Value& v) -> std::string {
    return v;
  };
}

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

} // namespace plotfx


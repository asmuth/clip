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
#include "format.h"
#include "common/config_helpers.h"
#include "common/utils/UnixTime.h"

namespace plotfx {

Formatter format_decimal_scientific(size_t precision) {
  Formatter f;
  f.format_number = [precision] (double v) -> std::string {
    std::stringstream s;
    s << std::scientific << std::setprecision(precision) << v;
    return s.str();
  };

  return f;
}

ReturnCode confgure_format_decimal_scientific(
    const plist::Property& prop,
    Formatter* formatter) {
  if (!plist::is_enum(prop, "scientific")) {
    return ERROR_INVALID_ARGUMENT;
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
        return ERROR_INVALID_ARGUMENT;
      }
    default:
      return ERROR_INVALID_ARGUMENT;
  }

  *formatter = format_decimal_scientific(precision);
  return OK;
}

Formatter format_decimal_fixed(size_t precision) {
  Formatter f;
  f.format_number = [precision] (double v) -> std::string {
    std::stringstream s;
    s << std::fixed << std::setprecision(precision) << v;
    return s.str();
  };

  return f;
}

ReturnCode confgure_format_decimal_fixed(
    const plist::Property& prop,
    Formatter* formatter) {
  if (!plist::is_enum(prop, "fixed")) {
    return ERROR_INVALID_ARGUMENT;
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
        return ERROR_INVALID_ARGUMENT;
      }
    default:
      return ERROR_INVALID_ARGUMENT;
  }

  *formatter = format_decimal_fixed(precision);
  return OK;
}

Formatter format_datetime(const std::string& fmt) {
  Formatter f;
  f.format_number = [fmt] (double v) -> std::string {
    UnixTime v_t(v * 1000000);
    return v_t.toString(fmt.c_str());
  };

  return f;
}

ReturnCode confgure_format_datetime(
    const plist::Property& prop,
    Formatter* formatter) {
  if (!plist::is_enum(prop, "datetime")) {
    return ERROR_INVALID_ARGUMENT;
  }

  std::string fmtspec = "%Y-%m-%d %H:%M:%S"; // FIXME improved auto format
  switch (prop.size()) {
    case 0:
      break;
    case 1:
      fmtspec = prop[0];
      break;
    default:
      return ERROR_INVALID_ARGUMENT;
  }

  *formatter = format_datetime(fmtspec);
  return OK;
}

ReturnCode confgure_format(
    const plist::Property& prop,
    Formatter* formatter) {
  if (prop.size() < 1) {
    return ERROR_INVALID_ARGUMENT;
  }

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

  return OK;
}

} // namespace plotfx


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
#include "data.h"
#include "sexpr_util.h"
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

ReturnCode format_configure_decimal_scientific(
    const Expr* expr,
    Formatter* formatter) {
  auto args = expr_collect(expr);

  uint32_t precision = 1;
  switch (args.size()) {
    case 0:
      break;
    case 1:
      try {
        precision = std::stod(expr_get_value(args[0]));
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
  return [precision] (const std::string& v) -> std::string {
    std::stringstream s;
    s << std::fixed << std::setprecision(precision) << value_to_float(v);
    return s.str();
  };
}

ReturnCode format_configure_decimal_fixed(
    const Expr* expr,
    Formatter* formatter) {
  auto args = expr_collect(expr);

  uint32_t precision = 1;
  switch (args.size()) {
    case 0:
      break;
    case 1:
      try {
        precision = std::stod(expr_get_value(args[0]));
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
  return [fmt] (const std::string& v) -> std::string {
    UnixTime v_t(value_to_float(v) * 1000000);
    return v_t.toString(fmt.c_str());
  };
}

ReturnCode format_configure_datetime(
    const Expr* expr,
    Formatter* formatter) {
  auto args = expr_collect(expr);

  std::string fmtspec = "%Y-%m-%d %H:%M:%S"; // FIXME improved auto format
  switch (args.size()) {
    case 0:
      break;
    case 1:
      fmtspec = expr_get_value(args[0]);
      break;
    default:
      return ERROR;
  }

  *formatter = format_datetime(fmtspec);
  return OK;
}

Formatter format_string() {
  return [] (const std::string& v) -> std::string {
    return v;
  };
}

ReturnCode format_configure_string(
    const Expr* expr,
    Formatter* formatter) {
  *formatter = format_string();
  return OK;
}

ReturnCode format_configure(
    const Expr* expr,
    Formatter* formatter) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list but got: {}",
        "..."); // FIXME
  }

  expr = expr_get_list(expr);

  if (expr_is_value(expr, "fixed")) {
    return format_configure_decimal_fixed(expr_next(expr), formatter);
  }

  if (expr_is_value(expr, "scientific")) {
    return format_configure_decimal_scientific(expr_next(expr), formatter);
  }

  if (expr_is_value(expr, "datetime")) {
    return format_configure_datetime(expr_next(expr), formatter);
  }

  if (expr_is_value(expr, "string")) {
    return format_configure_string(expr_next(expr), formatter);
  }

  return errorf(
      ERROR,
      "invalid value '{}', expected one of: \n"
      "  - fixed\n"
      "  - scientific\n",
      "  - datetime\n",
      "  - string\n",
      expr_inspect(expr));
}

} // namespace fviz


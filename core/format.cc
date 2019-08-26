/**
 * This file is part of the "clip" project
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

namespace clip {

std::string format_superscript(int n) {
  std::stringstream sstream;

  for (const auto& c : std::to_string(n)) {
    switch (c) {
      case '0':
        sstream << "⁰";
        break;
      case '1':
        sstream << "¹";
        break;
      case '2':
        sstream << "²";
        break;
      case '3':
        sstream << "³";
        break;
      case '4':
        sstream << "⁴";
        break;
      case '5':
        sstream << "⁵";
        break;
      case '6':
        sstream << "⁶";
        break;
      case '7':
        sstream << "⁷";
        break;
      case '8':
        sstream << "⁸";
        break;
      case '9':
        sstream << "⁹";
        break;
      case '-':
        sstream << "⁻";
        break;
      default:
        sstream << "�";
        break;
    }
  }

  return sstream.str();
}

Formatter format_decimal_scientific(size_t precision) {
  return [precision] (size_t idx, const std::string& vs) -> std::string {
    auto v = value_to_float(vs);
    if (v == 0) {
      return "0";
    }

    auto s = v >= 0 ? 1 : -1;
    auto e = int(log10(v / s));
    auto x = v / pow(10, e);

    std::stringstream sstream;
    sstream
        << std::fixed
        << std::setprecision(precision)
        << x
        << " × 10"
        << format_superscript(e);

    return sstream.str();
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
  return [precision] (size_t idx, const std::string& v) -> std::string {
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
  return [fmt] (size_t idx, const std::string& v) -> std::string {
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

Formatter format_base_fixed(unsigned base, size_t precision) {
  return [base, precision] (size_t idx, const std::string& vs) -> std::string {
    auto v = value_to_float(vs);
    if (v == 0) {
      return "0";
    }

    std::stringstream sstream;
    sstream
        << base
        << std::fixed
        << std::setprecision(precision)
        << format_superscript(log(v) / log(base));

    return sstream.str();
  };
}

ReturnCode format_configure_base_fixed(
    const Expr* expr,
    Formatter* formatter) {
  auto args = expr_collect(expr);

  double base;
  size_t precision = 1;
  switch (args.size()) {
    case 2:
      try {
        precision = std::stod(expr_get_value(args[1]));
      } catch (... ) {
        return ERROR;
      }
      /* fallthrough */
    case 1:
      try {
        base = std::stod(expr_get_value(args[0]));
      } catch (... ) {
        return ERROR;
      }
      break;
    default:
      return errorf(
          ERROR,
          "invalid number of arguments for 'base'; expected one or two, but got: {}",
          args.size());
  }

  *formatter = format_base_fixed(base, precision);
  return OK;
}

Formatter format_integer() {
  return [] (size_t idx, const std::string& vs) -> std::string {
    auto v = value_to_float(vs);
    return std::to_string(int(v));
  };
}

ReturnCode format_configure_integer(
    const Expr* expr,
    Formatter* formatter) {
  auto args = expr_collect(expr);
  if (args.size() != 0) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'integer'; expected zero, but got: {}",
        args.size());
  }

  *formatter = format_integer();
  return OK;
}

Formatter format_noop() {
  return [] (size_t idx, const std::string& v) -> std::string {
    return v;
  };
}

Formatter format_custom(const std::vector<std::string>& values) {
  return [values] (size_t idx, const std::string& v) -> std::string {
    if (values.empty()) {
      return "";
    } else {
      return values.at(idx % values.size());
    }
  };
}

ReturnCode format_configure_custom(
    const Expr* expr,
    Formatter* formatter) {
  std::vector<std::string> values;
  if (expr && expr_is_list(expr)) {
    if (auto rc = data_load_strings(expr, &values); !rc) {
      return rc;
    }
  } else {
    for (; expr; expr = expr_next(expr)) {
      values.emplace_back(expr_get_value(expr));
    }
  }

  *formatter = format_custom(values);
  return OK;
}

ReturnCode format_configure(
    const Expr* expr,
    Formatter* formatter) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list (<format>), but got: {}",
        expr_inspect(expr));
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

  if (expr_is_value(expr, "base")) {
    return format_configure_base_fixed(expr_next(expr), formatter);
  }

  if (expr_is_value(expr, "integer")) {
    return format_configure_integer(expr_next(expr), formatter);
  }

  if (expr_is_value(expr, "custom")) {
    return format_configure_custom(expr_next(expr), formatter);
  }

  return errorf(
      ERROR,
      "invalid value '{}', expected one of: \n"
      "  - fixed\n"
      "  - scientific\n",
      "  - datetime\n",
      "  - base\n",
      "  - integer\n",
      "  - custom\n",
      expr_inspect(expr));
}

} // namespace clip


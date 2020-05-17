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
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "utils/fileutil.h"
#include "utils/csv.h"
#include "utils/algo.h"
#include <iostream>

using namespace std::placeholders;

namespace clip {

ReturnCode expr_to_string(
    const Expr* expr,
    std::string* value) {
  if (!expr_is_value(expr)) {
    return error(ERROR, "expected value");
  }

  *value = expr_get_value(expr);
  return OK;
}

ReturnCode expr_to_strings(
    const Expr* expr,
    std::vector<std::string>* values) {
  return expr_tov<std::string>(expr, std::bind(&expr_to_string, _1, _2), values);
}

ReturnCode expr_to_strings_flat(
    const Expr* expr,
    std::vector<std::string>* values) {
  return expr_tov_flat<std::string>(expr, std::bind(&expr_to_string, _1, _2), values);
}

ReturnCode expr_to_stringset(
    const Expr* expr,
    std::set<std::string>* values) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a list, got: {}",
        expr_inspect(expr)); // FIXME
  }

  for (expr = expr_get_list(expr); expr; expr = expr_next(expr)) {
    std::string v;
    if (auto rc = expr_to_string(expr, &v); !rc) {
      return rc;
    }

    values->insert(std::move(v));
  }

  return OK;
}

ReturnCode expr_to_float64(
    const Expr* expr,
    double* value) {
  if (!expr_is_value(expr)) {
    return error(ERROR, "expected value");
  }

  try {
    *value = std::stod(expr_get_value(expr));
  } catch (... ) {
    return errorf(ERROR, "invalid number: {}", expr_get_value(expr));
  }

  return OK;
}

ReturnCode expr_to_float64_opt(
    const Expr* expr,
    Option<double>* value) {
  double v;
  if (auto rc = expr_to_float64(expr, &v); !rc) {
    return rc;
  }

  *value = v;
  return OK;
}

ReturnCode expr_to_float64_opt_pair(
    const Expr* expr,
    Option<double>* v1,
    Option<double>* v2) {
  if (expr  && expr_is_list(expr)) {
    expr = expr_get_list(expr);
  } else {
    return errorf(
        ERROR,
        "argument error; expected a list, got: {}",
        expr_inspect(expr));
  }

  for (size_t i = 0; i < 2; ++i) {
    if (!expr || !expr_is_value(expr)) {
      return errorf(
          ERROR,
          "argument error; expected a value, got: {}",
          expr_inspect(expr));
    }

    if (auto rc = expr_to_float64_opt(expr, i == 0 ? v1 : v2); !rc) {
      return rc;
    }

    expr = expr_next(expr);
  }

  return OK;
}

ReturnCode expr_to_ratio(
    const Expr* expr,
    double* value) {
  return expr_to_float64(expr, value);
}

ReturnCode expr_to_switch(
    const Expr* expr,
    bool* value) {
  if (expr_is_value(expr, "on")) {
    *value = true;
    return OK;
  }

  if (expr_is_value(expr, "off")) {
    *value = false;
    return OK;
  }

  return errorf(
      ERROR,
      "argument error; expected 'on' or 'off', got: {}",
      expr_inspect(expr));
}

ReturnCode expr_to_number(
    const Expr* expr,
    const UnitConvMap& conv,
    Number* v) {
  if (!expr_is_value(expr)) {
    return errorf(ERROR, "invalid number: '{}'", expr_inspect(expr));
  }

  auto value_str = expr_get_value(expr);
  double value;
  size_t unit_pos;
  try {
    value = std::stod(value_str, &unit_pos);
  } catch (...) {
    return errorf(ERROR, "invalid number: '{}'", value_str);
  }

  if (unit_pos == value_str.size()) {
    return errorf(ERROR, "expected '{}' to be followed by a unit", value_str);
  }

  auto unit_str = value_str.substr(unit_pos);

  Unit unit;
  if (unit_parse(unit_str, &unit)) {
    auto conv_iter = conv.find(unit);
    if (conv_iter != conv.end()) {
      *v = conv_iter->second(value);
      return OK;
    }
  }

  return errorf(
      ERROR,
      "invalid unit: '{}'",
      unit_str);
}

ReturnCode expr_to_vec2(
    const Expr* expr,
    const UnitConvMap& conv1,
    const UnitConvMap& conv2,
    Vector2* v) {
  if (!expr_is_list(expr)) {
    return err_invalid_value(expr_inspect(expr), {"(<x> <y>)"});
  }

  auto args = expr_collect(expr_get_list(expr));
  if (args.size() != 2) {
    return err_invalid_nargs(args.size(), 2);
  }

  for (size_t i = 0; i < 2; ++i) {
    const auto& conv = i == 0 ? conv1 : conv2;

    if (auto rc = expr_to_number(args[i], conv, &v->at(i)); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode expr_to_stroke_style(
    const Expr* expr,
    StrokeStyle* style) {
  if (expr_is_value(expr, "none")) {
    style->line_width = from_unit(0);
    return OK;
  }

  return OK;
}

ReturnCode expr_to_copy(const Expr* e, ExprStorage* c) {
  *c = expr_clone(e, 1);
  return OK;
}

} // namespace clip


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
#include "sexpr_conv.h"
#include "utils/fileutil.h"
#include "utils/csv.h"
#include "utils/algo.h"
#include <iostream>

using namespace std::placeholders;

namespace fviz {

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
  return expr_tov<std::string>(expr, bind(&expr_to_string, _1, _2), values);
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
    std::optional<double>* value) {
  double v;
  if (auto rc = expr_to_float64(expr, &v); !rc) {
    return rc;
  }

  *value = v;
  return OK;
}

ReturnCode expr_to_measure(
    const Expr* expr,
    Measure* value) {
  if (!expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a value, got: {}",
        "..."); // FIXME
  }

  return parse_measure(expr_get_value(expr), value);
}

ReturnCode expr_to_measure_opt(
    const Expr* expr,
    std::optional<Measure>* value) {
  Measure v;
  if (auto rc = expr_to_measure(expr, &v); !rc) {
    return rc;
  }

  *value = v;
  return OK;
}

ReturnCode expr_to_measures(
    const Expr* expr,
    std::vector<Measure>* measures) {
  return expr_tov<Measure>(expr, bind(&expr_to_measure, _1, _2), measures);
}

ReturnCode expr_to_color(
    const Expr* expr,
    Color* value) {
  if (!expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a value, got: {}",
        "..."); // FIXME
  }

  const auto expr_value = expr_get_value(expr);
  if (StringUtil::beginsWith(expr_value, "#")) {
    if (value->parse(expr_value)) {
      return OK;
    }
  }

  return error(ERROR, "invalid color");
}

ReturnCode expr_to_color_opt(
    const Expr* expr,
    std::optional<Color>* var) {
  Color c;
  if (auto rc = expr_to_color(expr, &c); !rc) {
    return rc;
  }

  *var = c;
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

} // namespace fviz


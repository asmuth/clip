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
    return ReturnCode::error("EARG", "expected value");
  }

  *value = expr_get_value(expr);
  return OK;
}

ReturnCode expr_to_measure(
    const Expr* expr,
    Measure* value) {
  if (!expr_is_value(expr)) {
    return ReturnCode::errorf(
        "EARG",
        "argument error; expected a value, got: $0",
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
    return ReturnCode::errorf(
        "EARG",
        "argument error; expected a value, got: $0",
        "..."); // FIXME
  }

  const auto expr_value = expr_get_value(expr);
  if (StringUtil::beginsWith(expr_value, "#")) {
    if (value->parse(expr_value)) {
      return OK;
    }
  }

  return ReturnCode::error("EARG", "invalid color");
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

} // namespace fviz


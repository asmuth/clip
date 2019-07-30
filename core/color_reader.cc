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
#include "data.h"
#include "color_reader.h"

using namespace std::placeholders;

namespace fviz {

ReturnCode color_read(
    const Environment& env,
    const Expr* expr,
    Color* color) {
  if (!expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error to <color>; expected a value, got: {}",
        expr_inspect(expr));
  }

  const auto expr_value = expr_get_value(expr);
  if (StringUtil::beginsWith(expr_value, "#")) {
    if (color->parse(expr_value)) {
      return OK;
    }
  }

  return errorf(
      ERROR,
      "invalid color; expected hex color code, rgb(a) or palette index, got: '{}'",
      expr_inspect(expr));
}

ReturnCode color_read_string(
    const Environment& env,
    const std::string& value,
    Color* color) {
  if (StringUtil::beginsWith(value, "#")) {
    if (color->parse(value)) {
      return OK;
    }
  }

  return errorf(
      ERROR,
      "invalid color; expected hex color code or palette index, got: '{}'",
      value);
}

ReturnCode color_read_opt(
    const Environment& env,
    const Expr* expr,
    std::optional<Color>* color) {
  Color c;
  if (auto rc = color_read(env, expr, &c); !rc) {
    return rc;
  }

  *color = c;
  return OK;
}

ReturnCode color_map_read(
    const Environment& env,
    const Expr* expr,
    ColorMap* color_map) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument to <color-map>; expected a list, but got: '{}'",
        expr_inspect(expr));
  }

  expr = expr_get_list(expr);

  return errorf(
      ERROR,
      "invalid value to <color-map>; got '{}', but expected one of: \n"
      "  - steps\n"
      "  - gradient\n",
      expr_inspect(expr));
}

} // namespace fviz


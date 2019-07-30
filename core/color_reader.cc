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
#include "color_reader.h"

namespace fviz {

ReturnCode color_read(
    const Environment& env,
    const Expr* expr,
    Color* value) {
  if (!expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error to <color>; expected a value, got: {}",
        expr_inspect(expr));
  }

  const auto expr_value = expr_get_value(expr);
  if (StringUtil::beginsWith(expr_value, "#")) {
    if (value->parse(expr_value)) {
      return OK;
    }
  }

  return error(ERROR, "invalid color");
}

ReturnCode color_read_opt(
    const Environment& env,
    const Expr* expr,
    std::optional<Color>* var) {
  Color c;
  if (auto rc = color_read(env, expr, &c); !rc) {
    return rc;
  }

  *var = c;
  return OK;
}

} // namespace fviz


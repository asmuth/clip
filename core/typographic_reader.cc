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
#include "typographic_reader.h"
#include "sexpr_conv.h"

using namespace std::placeholders;

namespace fviz {

ReturnCode measure_read(
    const Expr* expr,
    Measure* value) {
  if (!expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a value, got: {}",
        expr_inspect(expr));
  }

  return parse_measure(expr_get_value(expr), value);
}

ReturnCode measure_read_opt(
    const Expr* expr,
    std::optional<Measure>* value) {
  Measure v;
  if (auto rc = measure_read(expr, &v); !rc) {
    return rc;
  }

  *value = v;
  return OK;
}

ReturnCode measure_read_list(
    const Expr* expr,
    std::vector<Measure>* measures) {
  return expr_tov<Measure>(expr, bind(&measure_read, _1, _2), measures);
}

} // namespace fviz


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
#include "typographic_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <functional>

using namespace std::placeholders;
using std::bind;

namespace clip {

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

ReturnCode measure_readn(
    const Context* ctx,
    const Expr* expr,
    Measure* value) {
  if (!expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a value, got: {}",
        expr_inspect(expr));
  }

  if (auto rc = parse_measure(expr_get_value(expr), value); !rc) {
    return rc;
  }

  MeasureConv conv;
  conv.dpi = layer_get_dpi(ctx);
  conv.font_size = layer_get_font_size(ctx);
  measure_normalize(conv, value);

  return OK;
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

ReturnCode measure_map_read_linear(
    const Context* ctx,
    const Expr* expr,
    MeasureMap* measure_map) {
  auto args = expr_collect(expr);
  if (args.size() != 2) {
    return errorf(
        ERROR,
        "invalid number of arguments for 'linear'; expected two, got: {}",
        args.size());
  }

  Measure begin;
  if (auto rc = measure_readn(ctx, args[0], &begin); !rc) {
    return rc;
  }

  Measure end;
  if (auto rc = measure_readn(ctx, args[1], &end); !rc) {
    return rc;
  }

  *measure_map = measure_map_linear(begin, end);
  return OK;
}

ReturnCode measure_map_read(
    const Context* ctx,
    const Expr* expr,
    MeasureMap* measure_map) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument to <measure-map>; expected a list, but got: '{}'",
        expr_inspect(expr));
  }

  expr = expr_get_list(expr);

  if (expr_is_value(expr, "linear")) {
    return measure_map_read_linear(ctx, expr_next(expr), measure_map);
  }

  return errorf(
      ERROR,
      "invalid value to <measure-map>; got '{}', but expected one of: \n"
      "  - linear\n",
      expr_inspect(expr));
}

} // namespace clip


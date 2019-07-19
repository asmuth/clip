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
#include "environment.h"
#include "core/format.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/font_lookup.h"

using namespace std::placeholders;

namespace fviz {

Environment::Environment() :
    screen_width(Unit::UNIT, 900),
    screen_height(Unit::UNIT, 480),
    dpi(96),
    background_color(Color::fromRGB(1,1,1)),
    text_color(Color::fromRGB(.2,.2,.2)),
    border_color(Color::fromRGB(.2,.2,.2)),
    font_size(from_pt(11, dpi)) {}

ReturnCode environment_setup_defaults(Environment* env) {
  if (!font_load(DefaultFont::HELVETICA_REGULAR, &env->font)) {
    return error(
        ERROR,
        "unable to find default sans-sans font (Helvetica/Arial)");
  }

  return OK;
}

ReturnCode environment_set(Environment* env, const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() < 2) {
    return error(ERROR, "'set' expects two arguments");
  }

  if (expr_is_value(args[0], "width")) {
    return expr_to_measure(args[1], &env->screen_width);
  }

  if (expr_is_value(args[0], "height")) {
    return OK;
    return expr_to_measure(args[1], &env->screen_height);
  }

  if (expr_is_value(args[0], "dpi")) {
    return OK;
    return expr_to_float64(args[1], &env->dpi);
  }

  return errorf(ERROR, "invalid property: {}", expr_get_value(args[0]));
}

ReturnCode environment_configure(Environment* env, const Expr* expr) {
  for (; expr; expr = expr_next(expr)) {
    if (!expr || !expr_is_list(expr)) {
      return errorf(ERROR, "expected a list but got: {}", expr_inspect(expr));
    }

    auto args = expr_get_list(expr);
    if (!args || !expr_is_value(args)) {
      return error(ERROR, "expected an element name");
    }

    auto arg0 = expr_get_value(args);
    if (arg0 == "set" ) {
      if (auto rc = environment_set(env, expr_next(args)); !rc) {
        return rc;
      }
    }
  }

  return OK;
}

} // namespace fviz


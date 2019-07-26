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

#include <functional>

using namespace std::placeholders;

namespace fviz {

Environment::Environment() :
    screen_width(Unit::UNIT, 900),
    screen_height(Unit::UNIT, 480),
    dpi(96),
    background_color(Color::fromRGB(1,1,1)),
    foreground_color(Color::fromRGB(0,0,0)),
    text_color(Color::fromRGB(0,0,0)),
    font_size(from_pt(11, dpi)) {}

ReturnCode environment_setup_defaults(Environment* env) {
  return try_chain({
    std::bind(&font_find, DefaultFont::ROMAN_SANS_MEDIUM, &env->font),
    std::bind(&font_find, DefaultFont::ROMAN_SANS_BOLD, &env->font_em),
    std::bind(&font_find, DefaultFont::ROMAN_SANS_BOLD, &env->font_symbol),
  });
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
    return expr_to_measure(args[1], &env->screen_height);
  }

  if (expr_is_value(args[0], "dpi")) {
    return expr_to_float64(args[1], &env->dpi);
  }

  if (expr_is_value(args[0], "background-color")) {
    return expr_to_color(args[1], &env->background_color);
  }

  if (expr_is_value(args[0], "foreground-color")) {
    return expr_to_color(args[1], &env->foreground_color);
  }

  if (expr_is_value(args[0], "text-color")) {
    return expr_to_color(args[1], &env->text_color);
  }

  if (expr_is_value(args[0], "font")) {
    return font_find_expr(args[1], &env->font);
  }

  if (expr_is_value(args[0], "font-emphasis")) {
    return font_find_expr(args[1], &env->font_em);
  }

  if (expr_is_value(args[0], "font-symbols")) {
    return font_find_expr(args[1], &env->font_symbol);
  }

  if (expr_is_value(args[0], "font-size")) {
    return expr_to_measure(args[1], &env->font_size);
  }

  if (expr_is_value(args[0], "margin-top")) {
    return expr_to_measure(args[1], &env->margins[0]);
  }

  if (expr_is_value(args[0], "margin-right")) {
    return expr_to_measure(args[1], &env->margins[1]);
  }

  if (expr_is_value(args[0], "margin-bottom")) {
    return expr_to_measure(args[1], &env->margins[2]);
  }

  if (expr_is_value(args[0], "margin-left")) {
    return expr_to_measure(args[1], &env->margins[3]);
  }

  if (expr_is_value(args[0], "margin")) {
    return expr_calln(args[1], {
      bind(&expr_to_measure, _1, &env->margins[0]),
      bind(&expr_to_measure, _1, &env->margins[1]),
      bind(&expr_to_measure, _1, &env->margins[2]),
      bind(&expr_to_measure, _1, &env->margins[3]),
    });
  }

  return errorf(ERROR, "invalid property: {}", expr_get_value(args[0]));
}

ReturnCode environment_configure(Environment* env, const Expr* expr) {
  /* parse options */
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

  /* convert units */
  convert_unit_typographic(env->dpi, env->font_size, &env->font_size);

  for (auto& m : env->margins) {
    convert_unit_typographic(env->dpi, env->font_size, &m);
  }

  return OK;
}

} // namespace fviz


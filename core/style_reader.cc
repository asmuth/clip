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
#include "style_reader.h"
#include "sexpr_util.h"
#include "color_reader.h"

#include <functional>

using std::bind;
using namespace std::placeholders;

namespace fviz {

ReturnCode stroke_style_read_dash(
    const Environment& env,
    const Expr* expr,
    StrokeStyle* style) {
  style->dash_type = StrokeStyle::DASH;
  style->dash_pattern = {from_pt(2, env.dpi), from_pt(2, env.dpi)};

  if (!expr) {
    return OK;
  }

  return expr_walk_map(expr, {
    {"pattern", expr_tov_fn<Measure>(bind(&measure_readn, env, _1, _2), &style->dash_pattern)},
    {"offset", bind(&measure_readn, env, _1, &style->dash_offset)},
  });
}

ReturnCode stroke_style_read(
    const Environment& env,
    const Expr* expr,
    StrokeStyle* style) {
  if (expr_is_value(expr, "none") ||
      expr_is_list(expr, "none")) {
    style->line_width = from_unit(0);
    return OK;
  }

  if (expr_is_value(expr, "solid") ||
      expr_is_list(expr, "solid")) {
    style->dash_type = StrokeStyle::SOLID;
    return OK;
  }

  if (expr_is_value(expr, "dash") ||
      expr_is_value(expr, "dashed") ||
      expr_is_list(expr, "dash") ||
      expr_is_list(expr, "dashed")) {
    return stroke_style_read_dash(env, expr_get_list_tail(expr), style);
  }

  return errorf(
      ERROR,
      "invalid <stroke-style>; expected one of (none, solid), "
      "but got: '{}'",
      expr_inspect(expr));
}

ReturnCode fill_style_read_solid(
    const Environment& env,
    const Expr* expr,
    FillStyle* style) {
  Color c;

  if (auto rc = color_read(env, expr, &c); !rc) {
    return rc;
  }

  *style = fill_style_solid(c);
  return OK;
}

ReturnCode fill_style_read_hatch(
    const Environment& env,
    const Expr* expr,
    FillStyle* style) {
  Color color;
  double angle_deg = 45;
  Measure offset = from_unit(0);
  Measure stride = from_pt(3, env.dpi);
  Measure width = from_pt(1, env.dpi);

  if (expr) {
    auto read_rc = expr_walk_map(expr, {
      {"color", bind(&color_read, env, _1, &color)},
      {"angle", bind(&expr_to_float64, _1, &angle_deg)},
      {"offset", bind(&measure_readn, env, _1, &offset)},
      {"stride", bind(&measure_readn, env, _1, &stride)},
      {"width", bind(&measure_readn, env, _1, &width)},
    });

    if (!read_rc) {
      return read_rc;
    }
  }

  *style = fill_style_hatch(
      color,
      angle_deg,
      offset,
      stride,
      width);

  return OK;
}

ReturnCode fill_style_read(
    const Environment& env,
    const Expr* expr,
    FillStyle* style) {
  if (expr_is_value(expr, "none") ||
      expr_is_list(expr, "none")) {
    *style = fill_style_none();
    return OK;
  }

  if (expr_is_value(expr, "hatch") ||
      expr_is_value(expr, "hatched") ||
      expr_is_list(expr, "hatch") ||
      expr_is_list(expr, "hatched")) {
    return fill_style_read_hatch(env, expr_get_list_tail(expr), style);
  }

  if (expr_is_value(expr)) {
    return fill_style_read_solid(env, expr, style);
  }

  return errorf(
      ERROR,
      "invalid <fill-style>; expected a color or one of (none), "
      "but got: '{}'",
      expr_inspect(expr));
}

} // namespace fviz


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
#include "context.h"
#include "data.h"
#include "style_reader.h"
#include "sexpr_util.h"
#include "color_reader.h"

#include <functional>

using std::bind;
using namespace std::placeholders;

namespace clip {

ReturnCode stroke_style_read_dash(
    const Context* ctx,
    const Expr* expr,
    StrokeStyle* style) {
  style->dash_type = StrokeStyle::DASH;
  style->dash_pattern = {from_pt(2, layer_get_dpi(ctx)), from_pt(2, layer_get_dpi(ctx))};

  if (!expr) {
    return OK;
  }

  return expr_walk_map(expr, {
    {"pattern", expr_tov_fn<Measure>(std::bind(&measure_readn, ctx, _1, _2), &style->dash_pattern)},
    {"offset", std::bind(&measure_readn, ctx, _1, &style->dash_offset)},
  });
}

ReturnCode stroke_style_read(
    const Context* ctx,
    const Expr* expr,
    StrokeStyle* style) {
  if (expr_is_value(expr, "none") ||
      expr_is_list(expr, "none")) {
    style->line_width = Number(0);
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
    return stroke_style_read_dash(ctx, expr_get_list_tail(expr), style);
  }

  return errorf(
      ERROR,
      "invalid <stroke-style>; expected one of (none, solid), "
      "but got: '{}'",
      expr_inspect(expr));
}

ReturnCode fill_style_read_solid(
    const Context* ctx,
    const Expr* expr,
    FillStyle* style) {
  Color c;

  if (auto rc = color_read(ctx, expr, &c); !rc) {
    return rc;
  }

  style->color = c;
  return OK;
}

ReturnCode fill_style_read_hatch(
    const Context* ctx,
    const Expr* expr,
    FillStyle* style) {
  Color color;
  double angle_deg = -45;
  Measure offset = from_unit(0);
  Measure stride = from_pt(3, layer_get_dpi(ctx));
  Measure width = from_pt(1, layer_get_dpi(ctx));

  if (expr) {
    auto read_rc = expr_walk_map(expr, {
      {"color", std::bind(&color_read, ctx, _1, &color)},
      {"angle", std::bind(&expr_to_float64, _1, &angle_deg)},
      {"offset", std::bind(&measure_readn, ctx, _1, &offset)},
      {"stride", std::bind(&measure_readn, ctx, _1, &stride)},
      {"width", std::bind(&measure_readn, ctx, _1, &width)},
    });

    if (!read_rc) {
      return read_rc;
    }
  }

  style->color = color;
  style->hatch = true;
  style->hatch_angle_deg = angle_deg;
  style->hatch_offset = offset;
  style->hatch_stride = stride;
  style->hatch_width = width;

  return OK;
}

ReturnCode fill_style_read(
    const Context* ctx,
    const Expr* expr,
    FillStyle* style) {
  if (expr_is_value(expr, "none") ||
      expr_is_list(expr, "none")) {
    style->color = Option<Color>();
    return OK;
  }

  if (expr_is_value(expr, "hatch") ||
      expr_is_value(expr, "hatched") ||
      expr_is_list(expr, "hatch") ||
      expr_is_list(expr, "hatched")) {
    return fill_style_read_hatch(ctx, expr_get_list_tail(expr), style);
  }

  if (expr_is_value(expr) || expr_is_list(expr)) {
    return fill_style_read_solid(ctx, expr, style);
  }

  return errorf(
      ERROR,
      "invalid <fill-style>; expected a color, 'hatch' or 'none', "
      "but got: '{}'",
      expr_inspect(expr));
}

ReturnCode style_read_fill_solid(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  Color color;
  if (auto rc = expr_to_color(expr, layer, &color); !rc) {
    return rc;
  }

  styles->fill_solid.push_back(draw_style::fill_solid {
    .color = color
  });

  return OK;
}

ReturnCode style_read_fill_hatch(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  draw_style::fill_hatch style;

  style.angle_deg = -45;
  style.width = unit_from_pt(2, layer_get_dpi(layer));
  style.stride = unit_from_pt(6, layer_get_dpi(layer));

  auto rc = expr_walk_map(expr, {
    {"color", std::bind(&expr_to_color, _1, layer, &style.color)},
    {"width", std::bind(&expr_to_size, _1, layer, &style.width)},
    {"stride", std::bind(&expr_to_size, _1, layer, &style.stride)},
    {"angle", std::bind(&expr_to_float64, _1, &style.angle_deg)},
    {"offset", std::bind(&expr_to_size, _1, layer, &style.offset)},
  });

  if (!rc) {
    return rc;
  }

  styles->fill_hatch.push_back(style);
  return OK;
}

ReturnCode style_read_fill(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  if (expr_is_list(expr, "solid")) {
    return style_read_fill_solid(expr_next(expr_get_list(expr)), layer, styles);
  }

  if (expr_is_list(expr, "hatch")) {
    return style_read_fill_hatch(expr_next(expr_get_list(expr)), layer, styles);
  }

  return style_read_fill_solid(expr, layer, styles);
}

ReturnCode style_read_stroke_solid(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  draw_style::stroke_solid style;
  style.width = unit_from_pt(1, layer_get_dpi(layer));

  auto rc = expr_walk_map(expr, {
    {"color", std::bind(&expr_to_color, _1, layer, &style.color)},
    {"width", std::bind(&expr_to_size, _1, layer, &style.width)},
  });

  if (!rc) {
    return rc;
  }

  styles->stroke_solid.push_back(style);
  return OK;
}

ReturnCode style_read_stroke_solid_short(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  auto args = expr_collect(expr);
  if (args.size() != 2) {
    return err_invalid_nargs(args.size(), 2);
  }

  Number width;
  if (auto rc = expr_to_size(args[0], layer, &width); !rc) {
    return rc;
  }

  Color color;
  if (auto rc = expr_to_color(args[1], layer, &color); !rc) {
    return rc;
  }

  styles->stroke_solid.push_back(draw_style::stroke_solid {
    .color = color,
    .width = width
  });

  return OK;
}

ReturnCode style_read_stroke_dash(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  draw_style::stroke_dash style;
  style.width = unit_from_pt(1, layer_get_dpi(layer));
  style.pattern = {
    unit_from_pt(4, layer_get_dpi(layer)),
    unit_from_pt(4, layer_get_dpi(layer)),
  };

  auto rc = expr_walk_map(expr, {
    {"color", std::bind(&expr_to_color, _1, layer, &style.color)},
    {"width", std::bind(&expr_to_size, _1, layer, &style.width)},
    {"pattern", expr_tov_fn<Number>(std::bind(&expr_to_size, _1, layer, _2), &style.pattern)},
    {"offset", std::bind(&expr_to_size, _1, layer, &style.offset)},
  });

  if (!rc) {
    return rc;
  }

  styles->stroke_dash.push_back(style);
  return OK;
}

ReturnCode style_read_stroke(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  if (expr_is_list(expr, "solid")) {
    return style_read_stroke_solid(expr_next(expr_get_list(expr)), layer, styles);
  }

  if (expr_is_list(expr, "dash")) {
    return style_read_stroke_dash(expr_next(expr_get_list(expr)), layer, styles);
  }

  if (expr_is_list(expr)) {
    return style_read_stroke_solid_short(expr_get_list(expr), layer, styles);
  }

  return err_invalid_value(expr_inspect(expr), {"solid", "dash"});
}

ReturnCode style_read(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles) {
  return expr_walk_map_wrapped(expr, {
    {"fill", std::bind(&style_read_fill, _1, layer, styles)},
    {"stroke", std::bind(&style_read_stroke, _1, layer, styles)},
  });
}

} // namespace clip


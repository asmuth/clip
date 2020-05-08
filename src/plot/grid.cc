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
#include "grid.h"
#include "context.h"
#include "format.h"
#include "layout.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "graphics/brush.h"
#include "scale.h"
#include "color_reader.h"
#include "style.h"
#include "style_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

using namespace std::placeholders;
using std::bind;

namespace clip::elements::plot::grid {

static const double kDefaultLineWidthPT = 1;

using GridlineLayout = std::function<ReturnCode (
    const ScaleConfig& domain,
    std::vector<double>*)>;

struct GridlineDefinition {
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  ScaleLayoutFn layout_x;
  ScaleLayoutFn layout_y;
  StrokeStyle stroke_style;
};

ReturnCode draw_grid(
    Context* ctx,
    std::shared_ptr<GridlineDefinition> config) {
  MeasureConv conv;
  conv.dpi = ctx->dpi;
  conv.font_size = ctx->font_size;
  conv.parent_size = ctx->font_size;

  measure_normalize(conv, &config->stroke_style.line_width);

  const auto& bbox = context_get_clip(ctx);

  ScaleLayout slayout_x;
  config->layout_x(config->scale_x, format_noop(), &slayout_x);

  ScaleLayout slayout_y;
  config->layout_y(config->scale_y, format_noop(), &slayout_y);

  for (const auto& tick : slayout_x.positions) {
    auto line_x = bbox.x + bbox.w * tick;

    draw_line(
        ctx,
        Point(line_x, bbox.y),
        Point(line_x, bbox.y + bbox.h),
        config->stroke_style);
  }

  for (const auto& tick : slayout_y.positions) {
    auto line_y = bbox.y + bbox.h * (1.0 - tick);

    draw_line(
        ctx,
        Point(bbox.x, line_y),
        Point(bbox.x + bbox.w, line_y),
        config->stroke_style);
  }

  return OK;
}

ReturnCode draw_grid(Context* ctx, const Expr* expr) {
  /* set defaults from ctxironment */
  auto c = std::make_shared<GridlineDefinition>();
  c->stroke_style.line_width = from_pt(1, ctx->dpi);
  c->stroke_style.color = Color::fromRGB(.9, .9, .9);
  c->scale_x = ctx->scale_x;
  c->scale_y = ctx->scale_y;

  /* parse properties */
  auto config_rc = expr_walk_map_wrapped(expr, {
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"tick-placement-x", bind(&scale_configure_layout, _1, &c->layout_x)},
    {"tick-placement-y", bind(&scale_configure_layout, _1, &c->layout_y)},
    {"scale-x", bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &c->scale_y)},
    {"scale-x-padding", bind(&expr_to_float64, _1, &c->scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &c->scale_y.padding)},
    {"color", bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"stroke-color", bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"stroke-width", bind(&measure_read, _1, &c->stroke_style.line_width)},
    {"stroke-style", bind(&stroke_style_read, ctx, _1, &c->stroke_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  scale_configure_layout_defaults(c->scale_x, nullptr, &c->layout_x);
  scale_configure_layout_defaults(c->scale_y, nullptr, &c->layout_y);

  return draw_grid(ctx, c);
}

} // namespace clip::elements::plot::grid


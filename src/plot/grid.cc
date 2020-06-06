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

namespace clip::plotgen {

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

ReturnCode plot_grid(
    Context* ctx,
    PlotConfig* plot,
    std::shared_ptr<GridlineDefinition> config) {
  const auto& bbox = plot_get_clip(plot, layer_get(ctx));

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

ReturnCode plot_grid(Context* ctx, PlotConfig* plot, const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  auto c = std::make_shared<GridlineDefinition>();
  c->stroke_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));
  c->stroke_style.color = Color::fromRGB(.9, .9, .9);
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;

  /* parse properties */
  auto config_rc = expr_walk_map(expr, {
    {"limit-x", std::bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", std::bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", std::bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", std::bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", std::bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", std::bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"tick-placement-x", std::bind(&scale_configure_layout, _1, &c->layout_x)},
    {"tick-placement-y", std::bind(&scale_configure_layout, _1, &c->layout_y)},
    {"scale-x", std::bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", std::bind(&scale_configure_kind, _1, &c->scale_y)},
    {"scale-x-padding", std::bind(&expr_to_float64, _1, &c->scale_x.padding)},
    {"scale-y-padding", std::bind(&expr_to_float64, _1, &c->scale_y.padding)},
    {"color", std::bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"stroke-color", std::bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"stroke-width", std::bind(&expr_to_size, _1, layer, &c->stroke_style.line_width)},
    {"stroke-style", std::bind(&stroke_style_read, ctx, _1, &c->stroke_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  scale_configure_layout_defaults(c->scale_x, nullptr, &c->layout_x);
  scale_configure_layout_defaults(c->scale_y, nullptr, &c->layout_y);

  return plot_grid(ctx, plot, c);
}

} // namespace clip::plotgen


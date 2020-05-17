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
#include "polygons.h"
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "context.h"
#include "color_reader.h"
#include "style_reader.h"
#include "typographic_map.h"
#include "typographic_reader.h"
#include "layout.h"
#include "marker.h"
#include "scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

struct PlotPolygonsConfig {
  std::vector<Poly2> polys;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  FillStyle fill_style;
  StrokeStyle stroke_style;
};

ReturnCode polygons_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotPolygonsConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->stroke_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));

  /* read arguments */
  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data", std::bind(&data_load_polys2, _1, &c->polys)},
    {"limit-x", std::bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", std::bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", std::bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", std::bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", std::bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", std::bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"scale-x", std::bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", std::bind(&scale_configure_kind, _1, &c->scale_y)},
    {
      "color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &c->stroke_style.color),
        std::bind(&fill_style_read_solid, ctx, _1, &c->fill_style),
      })
    },
    {"fill", std::bind(&fill_style_read, ctx, _1, &c->fill_style)},
    {"stroke-color", std::bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"stroke-width", std::bind(&expr_to_size, _1, layer, &c->stroke_style.line_width)},
    {"stroke-style", std::bind(&stroke_style_read, ctx, _1, &c->stroke_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* auto-scaling */
  for (const auto& p : c->polys) {
    for (const auto& v : p.boundary.vertices) {
      scale_fit(v.x, &c->scale_x);
      scale_fit(v.y, &c->scale_y);
    }
  }

  return OK;
}

ReturnCode polygons_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotPolygonsConfig conf;

  if (auto rc = polygons_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  /* apply transformation */
  const auto& bbox = plot_get_clip(plot, layer_get(ctx));

  for (auto& p : conf.polys) {
    for (auto& v : p.boundary.vertices) {
      v.x = std::clamp(scale_translate(conf.scale_x, v.x), 0.0, 1.0) * bbox.w + bbox.x;
      v.y = std::clamp(scale_translate(conf.scale_y, v.y), 0.0, 1.0) * bbox.h + bbox.y;
    }
  }

  /* draw polygons */
  for (const auto& poly : conf.polys) {
    draw_polygon(ctx, poly, conf.stroke_style, conf.fill_style);
  }

  return OK;
}

ReturnCode polygons_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotPolygonsConfig conf;
  return polygons_configure(ctx, plot, &conf, expr);
}


} // namespace clip::plotgen


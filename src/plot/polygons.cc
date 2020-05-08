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

namespace clip::plot {

ReturnCode polygons_draw(
    Context* ctx,
    const Expr* expr) {
  std::vector<Poly2> polys;
  ScaleConfig scale_x = ctx->scale_x;
  ScaleConfig scale_y = ctx->scale_y;
  FillStyle fill_style;
  StrokeStyle stroke_style;
  stroke_style.line_width = from_pt(1);

  /* read arguments */
  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data", bind(&data_load_polys2, _1, &polys)},
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &scale_x.min, &scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &scale_y.min, &scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &scale_y.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &scale_y)},
    {
      "color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &stroke_style.color),
        bind(&fill_style_read_solid, ctx, _1, &fill_style),
      })
    },
    {"fill", bind(&fill_style_read, ctx, _1, &fill_style)},
    {"stroke-color", bind(&color_read, ctx, _1, &stroke_style.color)},
    {"stroke-width", bind(&measure_read, _1, &stroke_style.line_width)},
    {"stroke-style", bind(&stroke_style_read, ctx, _1, &stroke_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* auto-scaling */
  for (const auto& p : polys) {
    for (const auto& v : p.boundary.vertices) {
      scale_fit(v.x, &scale_x);
      scale_fit(v.y, &scale_y);
    }
  }


  /* apply transformation */
  const auto& bbox = context_get_clip(ctx);

  for (auto& p : polys) {
    for (auto& v : p.boundary.vertices) {
      v.x = std::clamp(scale_translate(scale_x, v.x), 0.0, 1.0) * bbox.w + bbox.x;
      v.y = std::clamp(scale_translate(scale_y, v.y), 0.0, 1.0) * bbox.h + bbox.y;
    }
  }

  /* draw polygons */
  for (const auto& poly : polys) {
    draw_polygon(ctx, poly, stroke_style, fill_style);
  }

  return OK;
}

} // namespace clip::plot


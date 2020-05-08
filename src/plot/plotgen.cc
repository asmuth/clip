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
#include "eval.h"
#include "graphics/export_image.h"
#include "graphics/export_svg.h"
#include "layout.h"
#include "sexpr_parser.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "typographic_reader.h"

#include "plot/areas.h"
#include "plot/axis.h"
#include "plot/bars.h"
#include "plot/errorbars.h"
#include "plot/grid.h"
#include "plot/labels.h"
#include "plot/lines.h"
#include "plot/points.h"
#include "plot/polygons.h"
#include "plot/rectangles.h"
#include "plot/vectors.h"
#include "draw/rectangle.h"
#include "figure/legend.h"

using namespace std::placeholders;

namespace clip {

ReturnCode plotgen(
    Context* ctx,
    const Expr* expr) {
  return expr_walk_map(expr, {
    {"width", bind(&measure_read, _1, &ctx->width)},
    {"height", bind(&measure_read, _1, &ctx->height)},
    {"dpi", bind(&expr_to_float64, _1, &ctx->dpi)},
    {"font", expr_call_string_fn(bind(&font_load_best, _1, &ctx->font))},
    {"font-size", bind(&measure_read, _1, &ctx->font_size)},
    {"background", bind(&context_set_background, ctx, _1)},
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &ctx->scale_x.min, &ctx->scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &ctx->scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &ctx->scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &ctx->scale_y.min, &ctx->scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &ctx->scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &ctx->scale_y.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &ctx->scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &ctx->scale_y)},
    {"scale-x-padding", bind(&expr_to_float64, _1, &ctx->scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &ctx->scale_y.padding)},
    {
      "margin",
      expr_calln_fn({
        bind(&measure_read, _1, &ctx->margins[0]),
        bind(&measure_read, _1, &ctx->margins[1]),
        bind(&measure_read, _1, &ctx->margins[2]),
        bind(&measure_read, _1, &ctx->margins[3]),
      })
    },
    {"margin-top", bind(&measure_read, _1, &ctx->margins[0])},
    {"margin-right", bind(&measure_read, _1, &ctx->margins[1])},
    {"margin-bottom", bind(&measure_read, _1, &ctx->margins[2])},
    {"margin-left", bind(&measure_read, _1, &ctx->margins[3])},
    {"axes", bind(&elements::plot::axis::axis_add_all, ctx, _1)},
    {"areas", bind(&elements::plot::areas::areas_draw, ctx, _1)},
    {"axis", bind(&elements::plot::axis::axis_draw, ctx, _1)},
    {"bars", bind(&elements::plot::bars::bars_draw, ctx, _1)},
    {"errorbars", bind(&elements::plot::errorbars::errorbars_draw, ctx, _1)},
    {"grid", bind(&elements::plot::grid::draw_grid, ctx, _1)},
    {"labels", bind(&elements::plot::labels::labels_draw, ctx, _1)},
    {"lines", bind(&elements::plot::lines::draw_lines, ctx, _1)},
    {"points", bind(&elements::plot::points::points_draw, ctx, _1)},
    {"polygons", bind(&plot::polygons_draw, ctx, _1)},
    {"rectangles", bind(&elements::plot::rectangles::rectangles_draw, ctx, _1)},
    {"vectors", bind(&elements::plot::vectors::vectors_draw, ctx, _1)},
    {"legend", bind(&elements::legend::legend_draw, ctx, _1)},
  });
}

} // namespace clip


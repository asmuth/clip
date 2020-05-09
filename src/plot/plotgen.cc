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
#include "plotgen.h"
#include "eval.h"
#include "graphics/export_image.h"
#include "graphics/export_svg.h"
#include "layout.h"
#include "sexpr_parser.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "typographic_reader.h"
#include "color_reader.h"
#include "style_reader.h"

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

ReturnCode plot_eval(
    Context* ctx,
    const Expr* expr) {
  PlotConfig plot;

  return expr_walk_map(expr, {
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &plot.scale_x.min, &plot.scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &plot.scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &plot.scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &plot.scale_y.min, &plot.scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &plot.scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &plot.scale_y.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &plot.scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &plot.scale_y)},
    {"scale-x-padding", bind(&expr_to_float64, _1, &plot.scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &plot.scale_y.padding)},
    {
      "margin",
      expr_calln_fn({
        bind(&measure_read, _1, &plot.margins[0]),
        bind(&measure_read, _1, &plot.margins[1]),
        bind(&measure_read, _1, &plot.margins[2]),
        bind(&measure_read, _1, &plot.margins[3]),
      })
    },
    {"margin-top", bind(&measure_read, _1, &plot.margins[0])},
    {"margin-right", bind(&measure_read, _1, &plot.margins[1])},
    {"margin-bottom", bind(&measure_read, _1, &plot.margins[2])},
    {"margin-left", bind(&measure_read, _1, &plot.margins[3])},
    {"axes", bind(&plotgen::plot_axes, ctx, &plot, _1)},
    {"areas", bind(&plotgen::plot_areas, ctx, &plot, _1)},
    {"axis", bind(&plotgen::plot_axis, ctx, &plot, _1)},
    {"bars", bind(&plotgen::plot_bars, ctx, &plot,_1)},
    {"errorbars", bind(&plotgen::plot_errorbars, ctx, &plot, _1)},
    {"grid", bind(&plotgen::plot_grid, ctx, &plot, _1)},
    {"labels", bind(&plotgen::plot_labels, ctx, &plot, _1)},
    {"lines", bind(&plotgen::plot_lines, ctx, &plot, _1)},
    {"points", bind(&plotgen::plot_points, ctx, &plot, _1)},
    {"polygons", bind(&plotgen::plot_polygons, ctx, &plot, _1)},
    {"rectangles", bind(&plotgen::plot_rectangles, ctx, &plot, _1)},
    {"vectors", bind(&plotgen::plot_vectors, ctx, &plot, _1)},
    {"legend", bind(&plotgen::plot_legend, ctx, &plot, _1)},
    {"background", bind(&plot_set_background, ctx, &plot, _1)},
  });
}

Rectangle plot_get_clip(const PlotConfig* plot, const Layer* layer) {
  if (plot->layout_stack.empty()) {
    auto margins = plot->margins;
    for (auto& m : margins) {
      convert_unit_typographic(layer->dpi, layer_get_rem(layer), &m);
    }

    return layout_margin_box(
        Rectangle(0, 0, layer->width, layer->height),
        margins[0],
        margins[1],
        margins[2],
        margins[3]);
  } else {
    return plot->layout_stack.back();
  }
}

ReturnCode plot_set_background(
    Context* ctx,
    const PlotConfig* plot,
    const Expr* expr) {
  Rectangle rect = plot_get_clip(plot, layer_get(ctx));
  FillStyle fill_style;
  StrokeStyle stroke_style;
  stroke_style.line_width = from_pt(1);

  /* read arguments */
  auto config_rc = expr_walk_map_wrapped(expr, {
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

  Path p;
  path_add_rectangle(&p, rect);
  draw_path(ctx, p, stroke_style, fill_style);

  return OK;
}


} // namespace clip


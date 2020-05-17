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

ReturnCode plot_prepare(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  return expr_walk_map(expr, {
    /* scale configuration */
    {"limit-x", std::bind(&expr_to_float64_opt_pair, _1, &plot->scale_x.min, &plot->scale_x.max)},
    {"limit-x-min", std::bind(&expr_to_float64_opt, _1, &plot->scale_x.min)},
    {"limit-x-max", std::bind(&expr_to_float64_opt, _1, &plot->scale_x.max)},
    {"limit-y", std::bind(&expr_to_float64_opt_pair, _1, &plot->scale_y.min, &plot->scale_y.max)},
    {"limit-y-min", std::bind(&expr_to_float64_opt, _1, &plot->scale_y.min)},
    {"limit-y-max", std::bind(&expr_to_float64_opt, _1, &plot->scale_y.max)},
    {"scale-x", std::bind(&scale_configure_kind, _1, &plot->scale_x)},
    {"scale-y", std::bind(&scale_configure_kind, _1, &plot->scale_y)},
    {"scale-x-padding", std::bind(&expr_to_float64, _1, &plot->scale_x.padding)},
    {"scale-y-padding", std::bind(&expr_to_float64, _1, &plot->scale_y.padding)},

    /* geometry autoranging */
    {"areas", std::bind(plotgen::areas_autorange, ctx, plot, _1)},
    {"bars", std::bind(plotgen::bars_autorange, ctx, plot,_1)},
    {"errorbars", std::bind(plotgen::errorbars_autorange, ctx, plot, _1)},
    {"labels", std::bind(plotgen::labels_autorange, ctx, plot, _1)},
    {"lines", std::bind(plotgen::lines_autorange, ctx, plot, _1)},
    {"points", std::bind(plotgen::points_autorange, ctx, plot, _1)},
    {"polygons", std::bind(plotgen::polygons_autorange, ctx, plot, _1)},
    {"rectangles", std::bind(plotgen::rectangles_autorange, ctx, plot, _1)},
    {"vectors", std::bind(plotgen::vectors_autorange, ctx, plot, _1)},
  }, false);
}

ReturnCode plot_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  return expr_walk_map(expr, {
    /* margins */
    {
      "margin",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &plot->margins[0]),
        std::bind(&expr_to_size, _1, layer, &plot->margins[1]),
        std::bind(&expr_to_size, _1, layer, &plot->margins[2]),
        std::bind(&expr_to_size, _1, layer, &plot->margins[3]),
      })
    },
    {"margin-top", std::bind(&expr_to_size, _1, layer, &plot->margins[0])},
    {"margin-right", std::bind(&expr_to_size, _1, layer, &plot->margins[1])},
    {"margin-bottom", std::bind(&expr_to_size, _1, layer, &plot->margins[2])},
    {"margin-left", std::bind(&expr_to_size, _1, layer, &plot->margins[3])},

    /* axes/grid/legend */
    {"axes", std::bind(&plotgen::plot_axes, ctx, plot, _1)},
    {"axis", std::bind(&plotgen::plot_axis, ctx, plot, _1)},
    {"grid", std::bind(&plotgen::plot_grid, ctx, plot, _1)},
    {"background", std::bind(&plot_set_background, ctx, plot, _1)},
    {"legend", std::bind(&plotgen::plot_legend, ctx, plot, _1)},

    /* geometry elements */
    {"areas", std::bind(&plotgen::areas_draw, ctx, plot, _1)},
    {"bars", std::bind(&plotgen::bars_draw, ctx, plot,_1)},
    {"errorbars", std::bind(&plotgen::errorbars_draw, ctx, plot, _1)},
    {"labels", std::bind(&plotgen::labels_draw, ctx, plot, _1)},
    {"lines", std::bind(&plotgen::lines_draw, ctx, plot, _1)},
    {"points", std::bind(&plotgen::points_draw, ctx, plot, _1)},
    {"polygons", std::bind(&plotgen::polygons_draw, ctx, plot, _1)},
    {"rectangles", std::bind(&plotgen::rectangles_draw, ctx, plot, _1)},
    {"vectors", std::bind(&plotgen::vectors_draw, ctx, plot, _1)},
  }, false);
}

ReturnCode plot_eval(
    Context* ctx,
    const Expr* expr) {
  PlotConfig plot;

  /* scale setup */
  if (auto rc = plot_prepare(ctx, &plot, expr); !rc) {
    return rc;
  }

  /* execute drawing commands */
  if (auto rc = plot_draw(ctx, &plot, expr); !rc) {
    return rc;
  }

  return OK;
}

Rectangle plot_get_clip(const PlotConfig* plot, const Layer* layer) {
  if (plot->layout_stack.empty()) {
    return layout_margin_box(
        Rectangle(
            0,
            0,
            layer_get_width(*layer).value,
            layer_get_height(*layer).value),
        plot->margins[0].value,
        plot->margins[1].value,
        plot->margins[2].value,
        plot->margins[3].value);
  } else {
    return plot->layout_stack.back();
  }
}

ReturnCode plot_set_background(
    Context* ctx,
    const PlotConfig* plot,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  Rectangle rect = plot_get_clip(plot, layer_get(ctx));
  FillStyle fill_style;
  StrokeStyle stroke_style;
  stroke_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));

  /* read arguments */
  auto config_rc = expr_walk_map_wrapped(expr, {
    {
      "color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &stroke_style.color),
        std::bind(&fill_style_read_solid, ctx, _1, &fill_style),
      })
    },
    {"fill", std::bind(&fill_style_read, ctx, _1, &fill_style)},
    {"stroke-color", std::bind(&color_read, ctx, _1, &stroke_style.color)},
    {"stroke-width", std::bind(&expr_to_size, _1, layer, &stroke_style.line_width)},
    {"stroke-style", std::bind(&stroke_style_read, ctx, _1, &stroke_style)},
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


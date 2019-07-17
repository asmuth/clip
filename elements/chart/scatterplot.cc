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
#include "scatterplot.h"

#include "data.h"
#include "scale.h"
#include "element_factory.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>
#include <functional>

using namespace std::placeholders;

namespace fviz::elements::chart::scatterplot {

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig xscale;
  ScaleConfig yscale;

  ExprStorage xdata;
  ExprStorage ydata;

  std::set<std::string> axes;
  std::vector<ExprStorage> layout_opts;
  std::vector<ExprStorage> point_opts;
  std::vector<ExprStorage> axis_opts;
  std::vector<ExprStorage> axis_x_opts;
  std::vector<ExprStorage> axis_y_opts;
  ExprStorage grid_opts;
  std::vector<ExprStorage> grid_extra_opts;
  ExprStorage legend_opts;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"axes", bind(&expr_to_stringset, _1, &axes)},
    {
      "xdata",
      expr_calln_fn({
        bind(&data_load, _1, &x),
        bind(&expr_to_copy, _1, &xdata),
      }),
    },
    {
      "ydata",
      expr_calln_fn({
        bind(&data_load, _1, &y),
        bind(&expr_to_copy, _1, &ydata),
      }),
    },
    {"xmin", bind(&expr_to_float64_opt, _1, &xscale.min)},
    {"xmax", bind(&expr_to_float64_opt, _1, &xscale.max)},
    {"ymin", bind(&expr_to_float64_opt, _1, &yscale.min)},
    {"ymax", bind(&expr_to_float64_opt, _1, &yscale.max)},
    {"xscale", bind(&scale_configure_kind, _1, &xscale)},
    {"yscale", bind(&scale_configure_kind, _1, &yscale)},
    {
      "xlayout",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "layout", &axis_x_opts),
        bind(&expr_rewritev, _1, "xlayout", &grid_extra_opts),
      })
    },
    {
      "ylayout",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "layout", &axis_y_opts),
        bind(&expr_rewritev, _1, "ylayout", &grid_extra_opts),
      })
    },
    {"marker-size", bind(&expr_rewritev, _1, "size", &point_opts)},
    {"color", bind(&expr_rewritev, _1, "color", &point_opts)},
    {"colors", bind(&expr_rewritev, _1, "colors", &point_opts)},
    {"background", bind(&expr_rewritev, _1, "background-color", &layout_opts)},
    {"border", bind(&expr_rewritev, _1, "border", &axis_opts)},
    {"border-color", bind(&expr_rewritev, _1, "border-color", &axis_opts)},
    {"border-width", bind(&expr_rewritev, _1, "border-width", &axis_opts)},
    {"grid", bind(&expr_to_copy, _1, &grid_opts)},
    {"legend", bind(&expr_to_copy, _1, &legend_opts)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* check configuraton */
  if (!xdata) {
    return error(ERROR, "the 'xdata' argument is mandatory");
  }

  if (!ydata) {
    return error(ERROR, "the 'ydata' argument is mandatory");
  }

  /* scale autoconfig */
  for (const auto& v : x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &xscale);
    }
  }

  for (const auto& v : y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &yscale);
    }
  }

  auto xmin = expr_create_value(std::to_string(scale_min(xscale)));
  auto xmax = expr_create_value(std::to_string(scale_max(xscale)));
  auto ymin = expr_create_value(std::to_string(scale_min(yscale)));
  auto ymax = expr_create_value(std::to_string(scale_max(yscale)));

  auto chart_points = expr_build(
      "chart/points",
      "xdata",
      std::move(xdata),
      "ydata",
      std::move(ydata),
      "xmin",
      expr_clone(xmin.get()),
      "xmax",
      expr_clone(xmax.get()),
      "ymin",
      expr_clone(ymin.get()),
      "ymax",
      expr_clone(ymax.get()),
      std::move(point_opts));

  ExprStorage chart_gridlines;
  if (grid_opts) {
    chart_gridlines = expr_build(
        "chart/gridlines",
        "xmin",
        expr_clone(xmin.get()),
        "xmax",
        expr_clone(xmax.get()),
        "ymin",
        expr_clone(ymin.get()),
        "ymax",
        expr_clone(ymax.get()),
        expr_unwrap(std::move(grid_opts)),
        expr_clonev(grid_extra_opts));
  }

  ExprStorage chart_legend;
  if (legend_opts) {
    chart_legend = expr_build(
        "chart/legend",
        expr_unwrap(std::move(legend_opts)));
  }

  ExprStorage chart_axis_top;
  if (axes.empty() || axes.count("top")) {
    chart_axis_top = expr_build(
        "chart/axis-top",
        "min",
        expr_clone(xmin.get()),
        "max",
        expr_clone(xmax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_x_opts));
  }

  ExprStorage chart_axis_right;
  if (axes.empty() || axes.count("right")) {
    chart_axis_right = expr_build(
        "chart/axis-right",
        "min",
        expr_clone(ymin.get()),
        "max",
        expr_clone(ymax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_y_opts));
  }

  ExprStorage chart_axis_bottom;
  if (axes.empty() || axes.count("bottom")) {
    chart_axis_bottom = expr_build(
        "chart/axis-bottom",
        "min",
        expr_clone(xmin.get()),
        "max",
        expr_clone(xmax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_x_opts));
  }

  ExprStorage chart_axis_left;
  if (axes.empty() || axes.count("left")) {
    chart_axis_left = expr_build(
        "chart/axis-left",
        "min",
        expr_clone(ymin.get()),
        "max",
        expr_clone(ymax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_y_opts));
  }

  auto chart = expr_build(
      "chart/layout",
      std::move(layout_opts),
      "top",
      expr_build(std::move(chart_axis_top)),
      "right",
      expr_build(std::move(chart_axis_right)),
      "bottom",
      expr_build(std::move(chart_axis_bottom)),
      "left",
      expr_build(std::move(chart_axis_left)),
      "body",
      expr_build(
          std::move(chart_gridlines),
          std::move(chart_points),
          std::move(chart_legend)));

  std::cerr << expr_inspect(chart.get());
  return element_build_macro(env, std::move(chart), elem);
}

} // namespace fviz::elements::chart::scatterplot


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
#include "combochart.h"

#include "data.h"
#include "scale.h"
#include "element_factory.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>
#include <functional>

using namespace std::placeholders;

namespace fviz::elements::chart::combochart {

struct Geom {
  std::string elem_name;
  ExprStorage opts;
};

ReturnCode build_geom(
    const std::string& geom_elem_name,
    const Expr* expr,
    std::vector<Geom>* geoms,
    std::vector<Measure>* x,
    std::vector<Measure>* y) {
  auto opts = expr_clone(expr_get_list(expr));

  auto config_rc = expr_walk_map(opts.get(), {
    {"data-x", bind(&data_load, _1, x)},
    {"data-y", bind(&data_load, _1, y)},
    {"data-x-offset", bind(&data_load, _1, x)},
    {"data-y-offset", bind(&data_load, _1, y)},
  }, false);

  if (!config_rc) {
    return config_rc;
  }

  Geom g;
  g.elem_name = geom_elem_name;
  g.opts = std::move(opts);
  geoms->emplace_back(std::move(g));
  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::set<std::string> axes;
  std::vector<ExprStorage> layout_opts;
  std::vector<Geom> geoms;
  std::vector<ExprStorage> geom_opts;
  std::vector<ExprStorage> axis_opts;
  std::vector<ExprStorage> axis_x_opts;
  std::vector<ExprStorage> axis_y_opts;
  ExprStorage grid_opts;
  std::vector<ExprStorage> grid_extra_opts;
  ExprStorage legend_opts;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &scale_x.min, &scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &scale_y.min, &scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &scale_y.max)},
    {
      "scale-x",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &scale_x),
        bind(&expr_rewritev, _1, "scale-x", &geom_opts),
        bind(&expr_rewritev, _1, "scale", &axis_x_opts)
      })
    },
    {
      "scale-y",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &scale_y),
        bind(&expr_rewritev, _1, "scale-y", &geom_opts),
        bind(&expr_rewritev, _1, "scale", &axis_y_opts)
      })
    },
    {"scale-x-padding", bind(&expr_to_float64, _1, &scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &scale_y.padding)},
    {
      "ticks-x",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "ticks", &axis_x_opts),
        bind(&expr_rewritev, _1, "ticks-x", &grid_extra_opts),
      })
    },
    {
      "ticks-y",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "ticks", &axis_y_opts),
        bind(&expr_rewritev, _1, "ticks-y", &grid_extra_opts),
      })
    },
    {"axis-x-format", bind(&expr_rewritev, _1, "format", &axis_x_opts)},
    {"axis-y-format", bind(&expr_rewritev, _1, "format", &axis_y_opts)},
    {"axes", bind(&expr_to_stringset, _1, &axes)},
    {"areas", bind(&build_geom, "chart/areas", _1, &geoms, &x, &y)},
    {"bars", bind(&build_geom, "chart/bars", _1, &geoms, &x, &y)},
    {"lines", bind(&build_geom, "chart/lines", _1, &geoms, &x, &y)},
    {"labels", bind(&build_geom, "chart/labels", _1, &geoms, &x, &y)},
    {"points", bind(&build_geom, "chart/points", _1, &geoms, &x, &y)},
    {"grid", bind(&expr_to_copy, _1, &grid_opts)},
    {"legend", bind(&expr_to_copy, _1, &legend_opts)},
    {"margin", bind(&expr_rewritev, _1, "margin", &layout_opts)},
    {"margin-top", bind(&expr_rewritev, _1, "margin-top", &layout_opts)},
    {"margin-right", bind(&expr_rewritev, _1, "margin-right", &layout_opts)},
    {"margin-bottom", bind(&expr_rewritev, _1, "margin-bottom", &layout_opts)},
    {"margin-left", bind(&expr_rewritev, _1, "margin-left", &layout_opts)},
    {"background", bind(&expr_rewritev, _1, "background", &layout_opts)},
    {
      "border",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "border", &axis_opts),
        bind(&expr_rewritev, _1, "border", &layout_opts)
      })
    },
    {"border-top", bind(&expr_rewritev, _1, "border-top", &layout_opts)},
    {"border-right", bind(&expr_rewritev, _1, "border-right", &layout_opts)},
    {"border-bottom", bind(&expr_rewritev, _1, "border-bottom", &layout_opts)},
    {"border-left", bind(&expr_rewritev, _1, "border-left", &layout_opts)},
    {
      "border-color",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "border-color", &axis_opts),
        bind(&expr_rewritev, _1, "border-color", &layout_opts)
      })
    },
    {"border-top-color", bind(&expr_rewritev, _1, "border-top-color", &layout_opts)},
    {"border-right-color", bind(&expr_rewritev, _1, "border-right-color", &layout_opts)},
    {"border-bottom-color", bind(&expr_rewritev, _1, "border-bottom-color", &layout_opts)},
    {"border-left-color", bind(&expr_rewritev, _1, "border-left-color", &layout_opts)},
    {
      "border-width",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "border-width", &axis_opts),
        bind(&expr_rewritev, _1, "border-width", &layout_opts)
      })
    },
    {"border-top-width", bind(&expr_rewritev, _1, "border-top-width", &layout_opts)},
    {"border-right-width", bind(&expr_rewritev, _1, "border-right-width", &layout_opts)},
    {"border-bottom-width", bind(&expr_rewritev, _1, "border-bottom-width", &layout_opts)},
    {"border-left-width", bind(&expr_rewritev, _1, "border-left-width", &layout_opts)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* scale autoconfig */
  for (const auto& v : x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &scale_x);
    }
  }

  for (const auto& v : y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &scale_y);
    }
  }

  auto xmin = expr_create_value(std::to_string(scale_min(scale_x)));
  auto xmax = expr_create_value(std::to_string(scale_max(scale_x)));
  auto ymin = expr_create_value(std::to_string(scale_min(scale_y)));
  auto ymax = expr_create_value(std::to_string(scale_max(scale_y)));

  geom_opts.emplace_back(expr_create_value("limit-x-min"));
  geom_opts.emplace_back(expr_clone(xmin.get()));
  geom_opts.emplace_back(expr_create_value("limit-x-max"));
  geom_opts.emplace_back(expr_clone(xmax.get()));
  geom_opts.emplace_back(expr_create_value("limit-y-min"));
  geom_opts.emplace_back(expr_clone(ymin.get()));
  geom_opts.emplace_back(expr_create_value("limit-y-max"));
  geom_opts.emplace_back(expr_clone(ymax.get()));

  ExprStorage chart_gridlines;
  if (grid_opts) {
    chart_gridlines = expr_build(
        "chart/gridlines",
        "limit-x-min",
        expr_clone(xmin.get()),
        "limit-x-max",
        expr_clone(xmax.get()),
        "limit-y-min",
        expr_clone(ymin.get()),
        "limit-y-max",
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
        "limit-min",
        expr_clone(xmin.get()),
        "limit-max",
        expr_clone(xmax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_x_opts));
  }

  ExprStorage chart_axis_right;
  if (axes.empty() || axes.count("right")) {
    chart_axis_right = expr_build(
        "chart/axis-right",
        "limit-min",
        expr_clone(ymin.get()),
        "limit-max",
        expr_clone(ymax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_y_opts));
  }

  ExprStorage chart_axis_bottom;
  if (axes.empty() || axes.count("bottom")) {
    chart_axis_bottom = expr_build(
        "chart/axis-bottom",
        "limit-min",
        expr_clone(xmin.get()),
        "limit-max",
        expr_clone(xmax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_x_opts));
  }

  ExprStorage chart_axis_left;
  if (axes.empty() || axes.count("left")) {
    chart_axis_left = expr_build(
        "chart/axis-left",
        "limit-min",
        expr_clone(ymin.get()),
        "limit-max",
        expr_clone(ymax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_y_opts));
  }

  std::vector<ExprStorage> chart_geoms;
  for (const auto& g : geoms) {
    chart_geoms.emplace_back(
        expr_build(
            g.elem_name,
            expr_clonev(geom_opts),
            expr_clone(g.opts.get())));
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
          std::move(chart_geoms),
          std::move(chart_legend)));

  return element_build_macro(env, std::move(chart), elem);
}

} // namespace fviz::elements::chart::combochart


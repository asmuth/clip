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

  std::vector<ExprStorage> point_opts;

  auto config_rc = expr_walk_map(expr_next(expr), {
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
    {"yscale", bind(&scale_configure_kind, _1, &yscale)},
    {"xscale", bind(&scale_configure_kind, _1, &xscale)},
    {"marker-size", bind(&expr_rewritev, _1, "size", &point_opts)},
    {"color", bind(&expr_rewritev, _1, "color", &point_opts)},
    {"colors", bind(&expr_rewritev, _1, "colors", &point_opts)},
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

  auto chart_body = expr_build(
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

  auto chart_axis_top = expr_build(
      "chart/axis-top",
      "min",
      expr_clone(xmin.get()),
      "max",
      expr_clone(xmax.get()));

  auto chart_axis_right = expr_build(
      "chart/axis-right",
      "min",
      expr_clone(ymin.get()),
      "max",
      expr_clone(ymax.get()));

  auto chart_axis_bottom = expr_build(
      "chart/axis-bottom",
      "min",
      expr_clone(xmin.get()),
      "max",
      expr_clone(xmax.get()));

  auto chart_axis_left = expr_build(
      "chart/axis-left",
      "min",
      expr_clone(ymin.get()),
      "max",
      expr_clone(ymax.get()));

  auto chart = expr_build(
      "chart/layout",
      "top",
      expr_create_list(std::move(chart_axis_top)),
      "right",
      expr_create_list(std::move(chart_axis_right)),
      "bottom",
      expr_create_list(std::move(chart_axis_bottom)),
      "left",
      expr_create_list(std::move(chart_axis_left)),
      "body",
      expr_create_list(std::move(chart_body)));

  std::cerr << expr_inspect(chart.get());
  return element_build_macro(env, std::move(chart), elem);
}

} // namespace fviz::elements::chart::scatterplot


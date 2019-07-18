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
#include "linechart.h"

#include "data.h"
#include "scale.h"
#include "element_factory.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>
#include <functional>

using namespace std::placeholders;

namespace fviz::elements::chart::linechart {

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;

  ExprStorage data_x;
  ExprStorage data_y;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {
      "data-x",
      expr_calln_fn({
        bind(&data_load, _1, &x),
        bind(&expr_to_copy, _1, &data_x),
      }),
    },
    {
      "data-y",
      expr_calln_fn({
        bind(&data_load, _1, &y),
        bind(&expr_to_copy, _1, &data_y),
      }),
    },
    {"range-x-min", bind(&expr_to_float64_opt, _1, &scale_x.min)},
    {"range-x-max", bind(&expr_to_float64_opt, _1, &scale_x.max)},
    {"range-y-min", bind(&expr_to_float64_opt, _1, &scale_y.min)},
    {"range-y-max", bind(&expr_to_float64_opt, _1, &scale_y.max)},
    {"scale-y", bind(&scale_configure_kind, _1, &scale_y)},
    {"scale-x", bind(&scale_configure_kind, _1, &scale_x)},
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

  auto chart_body = expr_build(
      "chart/lines",
      "data-x",
      std::move(data_x),
      "data-y",
      std::move(data_y),
      "range-x-min",
      expr_clone(xmin.get()),
      "range-x-max",
      expr_clone(xmax.get()),
      "range-y-min",
      expr_clone(ymin.get()),
      "range-y-max",
      expr_clone(ymax.get()));

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

} // namespace fviz::elements::chart::linechart


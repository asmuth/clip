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
using std::bind;

namespace fviz::elements::chart::linechart {

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  std::vector<ExprStorage> layout_opts;
  std::vector<ExprStorage> geom_opts;

  auto config_rc = expr_walk_map(expr_next(expr), {
    /* linechart options */
    {"data-x", bind(&expr_rewritev, _1, "data-x", &geom_opts)},
    {"data-y", bind(&expr_rewritev, _1, "data-y", &geom_opts)},
    {"color", bind(&expr_rewritev, _1, "color", &geom_opts)},
    {"stroke", bind(&expr_rewritev, _1, "stroke", &geom_opts)},
    {"marker-size", bind(&expr_rewritev, _1, "marker-size", &geom_opts)},
    {"marker-sizes", bind(&expr_rewritev, _1, "marker-sizes", &geom_opts)},
    {"labels", bind(&expr_rewritev, _1, "labels", &geom_opts)},
    {"label-font-size", bind(&expr_rewritev, _1, "label-font-size", &geom_opts)},
    {"label-color", bind(&expr_rewritev, _1, "label-color", &geom_opts)},
    {"label-padding", bind(&expr_rewritev, _1, "label-padding", &geom_opts)},

    /* scale options */
    {"limit-x", bind(&expr_rewritev, _1, "limit-x", &layout_opts)},
    {"limit-x-min", bind(&expr_rewritev, _1, "limit-x-min", &layout_opts)},
    {"limit-x-max", bind(&expr_rewritev, _1, "limit-x-max", &layout_opts)},
    {"limit-y", bind(&expr_rewritev, _1, "limit-y", &layout_opts)},
    {"limit-y-min", bind(&expr_rewritev, _1, "limit-y-min", &layout_opts)},
    {"limit-y-max", bind(&expr_rewritev, _1, "limit-y-max", &layout_opts)},
    {"scale-x", bind(&expr_rewritev, _1, "scale-x", &layout_opts)},
    {"scale-y", bind(&expr_rewritev, _1, "scale-y", &layout_opts)},
    {"scale-x-padding", bind(&expr_rewritev, _1, "scale-x-padding", &layout_opts)},
    {"scale-y-padding", bind(&expr_rewritev, _1, "scale-y-padding", &layout_opts)},

    /* axis options */
    {"axes", bind(&expr_rewritev, _1, "axes", &layout_opts)},
    {"axis-top", bind(&expr_rewritev, _1, "axis-top", &layout_opts)},
    {"axis-right", bind(&expr_rewritev, _1, "axis-right", &layout_opts)},
    {"axis-bottom", bind(&expr_rewritev, _1, "axis-bottom", &layout_opts)},
    {"axis-left", bind(&expr_rewritev, _1, "axis-left", &layout_opts)},
    {"axis-x-label-format", bind(&expr_rewritev, _1, "axis-x-label-format", &layout_opts)},
    {"axis-y-label-format", bind(&expr_rewritev, _1, "axis-y-label-format", &layout_opts)},
    {"axis-x-label-placement", bind(&expr_rewritev, _1, "axis-x-label-placement", &layout_opts)},
    {"axis-y-label-placement", bind(&expr_rewritev, _1, "axis-y-label-placement", &layout_opts)},
    {"axis-x-title", bind(&expr_rewritev, _1, "axis-x-title", &layout_opts)},
    {"axis-y-title", bind(&expr_rewritev, _1, "axis-y-title", &layout_opts)},

    /* grid & legend */
    {"grid", bind(&expr_rewritev, _1, "grid", &layout_opts)},
    {"legend-overlay", bind(&expr_rewritev, _1, "legend-overlay", &layout_opts)},

    /* background, margins, borders */
    {"background", bind(&expr_rewritev, _1, "background", &layout_opts)},
    {"margin", bind(&expr_rewritev, _1, "margin", &layout_opts)},
    {"margin-top", bind(&expr_rewritev, _1, "margin-top", &layout_opts)},
    {"margin-right", bind(&expr_rewritev, _1, "margin-right", &layout_opts)},
    {"margin-bottom", bind(&expr_rewritev, _1, "margin-bottom", &layout_opts)},
    {"margin-left", bind(&expr_rewritev, _1, "margin-left", &layout_opts)},
    {"border", bind(&expr_rewritev, _1, "border", &layout_opts)},
    {"border-top", bind(&expr_rewritev, _1, "border-top", &layout_opts)},
    {"border-right", bind(&expr_rewritev, _1, "border-right", &layout_opts)},
    {"border-bottom", bind(&expr_rewritev, _1, "border-bottom", &layout_opts)},
    {"border-left", bind(&expr_rewritev, _1, "border-left", &layout_opts)},
    {"border-color", bind(&expr_rewritev, _1, "border-color", &layout_opts)},
    {"border-top-color", bind(&expr_rewritev, _1, "border-top-color", &layout_opts)},
    {"border-right-color", bind(&expr_rewritev, _1, "border-right-color", &layout_opts)},
    {"border-bottom-color", bind(&expr_rewritev, _1, "border-bottom-color", &layout_opts)},
    {"border-left-color", bind(&expr_rewritev, _1, "border-left-color", &layout_opts)},
    {"border-width", bind(&expr_rewritev, _1, "border-width", &layout_opts)},
    {"border-top-width", bind(&expr_rewritev, _1, "border-top-width", &layout_opts)},
    {"border-right-width", bind(&expr_rewritev, _1, "border-right-width", &layout_opts)},
    {"border-bottom-width", bind(&expr_rewritev, _1, "border-bottom-width", &layout_opts)},
    {"border-left-width", bind(&expr_rewritev, _1, "border-left-width", &layout_opts)},
  });

  if (!config_rc) {
    return config_rc;
  }

  auto chart = expr_build(
      "plot",
      std::move(layout_opts),
      "lines",
      expr_build(std::move(geom_opts)));

  return element_build_macro(env, chart.get(), elem);
}

} // namespace fviz::elements::chart::linechart


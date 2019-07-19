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
#include "lines.h"
#include "data.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "scale.h"
#include "layout.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;

namespace fviz::elements::chart::lines {

static const double kDefaultLineWidthPT = 2;

struct PlotLinesConfig {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<DataGroup> groups;
  std::vector<Color> colors;
  Measure line_width;
};

ReturnCode draw(
    std::shared_ptr<PlotLinesConfig> config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  /* draw lines */
  for (const auto& group : config->groups) {
    if (group.index.empty()) {
      continue;
    }

    Path path;
    for (auto i : group.index) {
      auto sx = clip.x + config->x[i];
      auto sy = clip.y + clip.h - config->y[i];

      if (i == group.index[0]) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    StrokeStyle style;
    style.line_width = config->line_width;
    style.color = config->colors.empty()
        ? Color{}
        : config->colors[group.index[0] % config->colors.size()];

    strokePath(layer, clip, path, style);
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* set defaults from environment */
  auto c = std::make_shared<PlotLinesConfig>();
  c->line_width = from_pt(kDefaultLineWidthPT);

  /* parse properties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"data-x", bind(&data_load, _1, &c->x)},
    {"data-y", bind(&data_load, _1, &c->y)},
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &c->scale_y)},
    {"scale-x-padding", bind(&expr_to_float64, _1, &c->scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &c->scale_y.padding)},
    {"color", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &c->colors)},
    {"colors", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &c->colors)},
    {"stroke", bind(&expr_to_measure, _1, &c->line_width)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* check configuraton */
  if (c->x.size() != c->y.size()) {
    return error(
        ERROR,
        "the length of the 'data-x' and 'data-y' properties must be equal");
  }

  /* group data */
  {
    DataGroup g;
    g.index = std::vector<size_t>(c->x.size());
    std::iota(g.index.begin(), g.index.end(), 0);
    c->groups.emplace_back(g);
  }

  /* scale autoconfig */
  for (const auto& v : c->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, c, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::lines


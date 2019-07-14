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
  auto config = std::make_shared<PlotLinesConfig>();
  config->line_width = from_pt(kDefaultLineWidthPT);

  /* parse properties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"xdata", bind(&expr_to_measures, _1, &config->x)},
    {"ydata", bind(&expr_to_measures, _1, &config->y)},
    {"xmin", bind(&expr_to_float64_opt, _1, &config->scale_x.min)},
    {"xmax", bind(&expr_to_float64_opt, _1, &config->scale_x.max)},
    {"xscale", bind(&scale_configure_kind, _1, &config->scale_x)},
    {"xscale-padding", bind(&expr_to_float64, _1, &config->scale_x.padding)},
    {"ymin", bind(&expr_to_float64_opt, _1, &config->scale_y.min)},
    {"ymax", bind(&expr_to_float64_opt, _1, &config->scale_y.max)},
    {"yscale", bind(&scale_configure_kind, _1, &config->scale_y)},
    {"yscale-padding", bind(&expr_to_float64, _1, &config->scale_y.padding)},
    {"color", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &config->colors)},
    {"colors", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &config->colors)},
    {"stroke", bind(&expr_to_measure, _1, &config->line_width)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* check configuraton */
  if (config->x.size() != config->y.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'xs' and 'ys' properties must be equal");
  }

  /* group data */
  {
    DataGroup g;
    g.index = std::vector<size_t>(config->x.size());
    std::iota(g.index.begin(), g.index.end(), 0);
    config->groups.emplace_back(g);
  }

  /* scale autoconfig */
  for (const auto& v : config->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &config->scale_y);
    }
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::lines


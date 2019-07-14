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
#include <numeric>
#include <fviz.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "source/config_helpers.h"

using namespace std::placeholders;

namespace fviz {
namespace plot {
namespace lines {

static const double kDefaultLineWidthPT = 2;

ReturnCode draw(
    PlotLinesConfig config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  /* draw lines */
  for (const auto& group : config.groups) {
    Path path;
    for (auto i : group.index) {
      auto sx = clip.x + config.x[i];
      auto sy = clip.y + clip.h - config.y[i];

      if (i == group.index[0]) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    StrokeStyle style;
    style.line_width = config.line_width;
    style.color = config.colors.empty()
        ? Color{}
        : config.colors[group.index[0] % config.colors.size()];

    strokePath(layer, clip, path, style);
  }

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    PlotLinesConfig* config) {
  /* set defaults from environment */
  config->scale_x = env.scale_x;
  config->scale_y = env.scale_y;
  config->line_width = from_pt(kDefaultLineWidthPT);

  ParserDefinitions pdefs = {
    {"xs", bind(&configure_measures, _1, &config->x)},
    {"ys", bind(&configure_measures, _1, &config->y)},
    {"scale-x", bind(&domain_configure, _1, &config->scale_x)},
    {"scale-x-min", bind(&configure_float_opt, _1, &config->scale_x.min)},
    {"scale-x-max", bind(&configure_float_opt, _1, &config->scale_x.max)},
    {"scale-x-padding", bind(&configure_float, _1, &config->scale_x.padding)},
    {"scale-y", bind(&domain_configure, _1, &config->scale_y)},
    {"scale-y-min", bind(&configure_float_opt, _1, &config->scale_y.min)},
    {"scale-y-max", bind(&configure_float_opt, _1, &config->scale_y.max)},
    {"scale-y-padding", bind(&configure_float, _1, &config->scale_y.padding)},
    {"color", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"colors", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"stroke", bind(&configure_measure, _1, &config->line_width)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
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
      domain_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->y) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_y);
    }
  }

  return OK;
}

} // namespace lines
} // namespace plot
} // namespace fviz


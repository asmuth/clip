/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "lines.h"
#include <numeric>
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "source/config_helpers.h"

using namespace std::placeholders;

namespace plotfx {
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

ReturnCode reflow(
    const PlotLinesConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  /* nothing to do */
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
} // namespace plotfx


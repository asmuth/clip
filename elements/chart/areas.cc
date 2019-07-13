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
#include "areas.h"
#include "plotfx.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "config_helpers.h"
#include "core/environment.h"

#include <numeric>

using namespace std::placeholders;

namespace plotfx {
namespace plot {
namespace area {

PlotAreaConfig::PlotAreaConfig() :
    direction(Direction::VERTICAL) {}

ReturnCode draw_horizontal(
    PlotAreaConfig config,
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
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  /* draw areas */
  Path path;

  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + clip.h - config.y[i];

    if (i == 0) {
      path.moveTo(sx, sy);
    } else {
      path.lineTo(sx, sy);
    }
  }

  auto x0 = clip.h * std::clamp(domain_translate(config.scale_x, 0), 0.0, 1.0);
  for (int i = config.x.size() - 1; i >= 0; --i) {
    auto sx = clip.x + (config.xoffset.empty() ? x0 : config.xoffset[i]);
    auto sy = clip.y + clip.h - config.y[i];
    path.lineTo(sx, sy);
  }

  path.closePath();

  FillStyle style;
  style.color = config.colors.empty()
      ? Color{}
      : config.colors[0];

  fillPath(layer, clip, path, style);

  return OK;
}

ReturnCode draw_vertical(
    PlotAreaConfig config,
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
        bind(&convert_unit_user, domain_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, domain_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  /* draw areas */
  Path path;

  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + clip.h - config.y[i];

    if (i == 0) {
      path.moveTo(sx, sy);
    } else {
      path.lineTo(sx, sy);
    }
  }

  auto y0 = clip.h * std::clamp(domain_translate(config.scale_y, 0), 0.0, 1.0);
  for (int i = config.x.size() - 1; i >= 0; --i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + clip.h - (config.yoffset.empty() ? y0 : config.yoffset[i]);
    path.lineTo(sx, sy);
  }

  path.closePath();

  FillStyle style;
  style.color = config.colors.empty()
      ? Color{}
      : config.colors[0];

  fillPath(layer, clip, path, style);

  return OK;
}

ReturnCode draw(
    const PlotAreaConfig& config,
    const LayoutInfo& layout,
    Layer* layer) {
  switch (config.direction) {
    case Direction::HORIZONTAL:
      return draw_horizontal(config, layout, layer);
    case Direction::VERTICAL:
      return draw_vertical(config, layout, layer);
    default:
      return ERROR;
  }
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    PlotAreaConfig* config) {
  /* set defaults from environment */
  config->scale_x = env.scale_x;
  config->scale_y = env.scale_y;

  /* parse properties */
  ParserDefinitions pdefs = {
    {"xs", bind(&configure_measures, _1, &config->x)},
    {"ys", bind(&configure_measures, _1, &config->y)},
    {"x-offsets", bind(&configure_measures, _1, &config->xoffset)},
    {"y-offsets", bind(&configure_measures, _1, &config->yoffset)},
    {"scale-x", bind(&domain_configure, _1, &config->scale_x)},
    {"scale-x-min", bind(&configure_float_opt, _1, &config->scale_x.min)},
    {"scale-x-max", bind(&configure_float_opt, _1, &config->scale_x.max)},
    {"scale-x-padding", bind(&configure_float, _1, &config->scale_x.padding)},
    {"scale-y", bind(&domain_configure, _1, &config->scale_y)},
    {"scale-y-min", bind(&configure_float_opt, _1, &config->scale_y.min)},
    {"scale-y-max", bind(&configure_float_opt, _1, &config->scale_y.max)},
    {"scale-y-padding", bind(&configure_float, _1, &config->scale_y.padding)},
    {"direction", bind(&configure_direction, _1, &config->direction)},
    {"color", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"colors", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
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

  if (!config->xoffset.empty() &&
      config->xoffset.size() != config->x.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'xs' and 'x-offsets' properties must be equal");
  }

  if (!config->yoffset.empty() &&
      config->yoffset.size() != config->y.size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'ys' and 'y-offsets' properties must be equal");
  }

  /* scale autoconfig */
  for (const auto& v : config->x) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->xoffset) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->y) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_y);
    }
  }

  for (const auto& v : config->yoffset) {
    if (v.unit == Unit::USER) {
      domain_fit(v.value, &config->scale_y);
    }
  }

  return OK;
}

} // namespace area
} // namespace plot
} // namespace plotfx


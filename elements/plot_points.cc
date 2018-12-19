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
#include "plot_points.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "common/config_helpers.h"

using namespace std::placeholders;

namespace plotfx {
namespace plot {
namespace points {

static const double kDefaultPointSizePT = 3;

ReturnCode draw(
    const PlotPointsConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i] * clip.w;
    auto sy = clip.y + (1.0 - config.y[i]) * clip.h;

    const auto& color = config.colors.empty()
        ? Color{}
        : config.colors[i % config.colors.size()];

    FillStyle style;
    style.color = color;

    // FIXME point style
    Path path;
    path.moveTo(sx + config.point_size, sy);
    path.arcTo(sx, sy, config.point_size, 0, M_PI * 2);
    fillPath(layer, clip, path, style);
  }

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Document& doc,
    const DataContext& data,
    const DomainMap& scales,
    PlotPointsConfig* config) {
  SeriesRef data_x = series_find(data.defaults, "x");
  SeriesRef data_y = series_find(data.defaults, "y");

  std::string scale_x = SCALE_DEFAULT_X;
  std::string scale_y = SCALE_DEFAULT_Y;

  std::optional<Color> color_default;
  SeriesRef color_var = series_find(data.defaults, "color");
  DomainConfig color_domain;
  ColorScheme color_palette;

  Measure point_size;

  static const ParserDefinitions pdefs = {
    {"x", configure_series_fn(&data_x)},
    {"x-scale", bind(&configure_string, _1, &scale_x)},
    {"y", configure_series_fn(&data_y)},
    {"y-scale", bind(&configure_string, _1, &scale_y)},
    {"color", configure_var(&color_var, configure_color_opt(&color_default))},
    {"size", bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &point_size)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  /* check dataset */
  if (!data_x || !data_y) {
    return ReturnCode::error("EARG", "the following properties are required: x, y");
  }

  if (data_x->size() != data_y->size()) {
    return ReturnCode::error(
        "EARG",
        "the length of the 'x' and 'y' properties must be equal");
  }

  /* fetch domains */
  auto domain_x = domain_find(scales, scale_x);
  if (!domain_x) {
    return ReturnCode::errorf("EARG", "scale not found: $0", scale_x);
  }

  auto domain_y = domain_find(scales, scale_y);
  if (!domain_y) {
    return ReturnCode::errorf("EARG", "scale not found: $0", scale_y);
  }

  /* return element */
  config->x = domain_translate(*domain_x, *data_x);
  config->y = domain_translate(*domain_y, *data_y);
  config->point_size = measure_or(point_size, from_pt(kDefaultPointSizePT, doc.dpi));
  config->colors = fallback(
      series_to_colors(color_var, color_domain, color_palette),
      color_default);

  return OK;
}

} // namespace points
} // namespace plot
} // namespace plotfx


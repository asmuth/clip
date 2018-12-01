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
#include "plot_lines.h"
#include <plotfx.h>
#include <graphics/path.h>
#include <graphics/brush.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include "common/config_helpers.h"

namespace plotfx {
namespace plot {
namespace lines {

PlotLinesConfig::PlotLinesConfig() :
    line_width(from_pt(2)) {}

ReturnCode draw_lines(
    const PlotConfig& plot,
    const PlotLinesConfig& series,
    const Rectangle& clip,
    Layer* layer) {
  const auto& domain_x = plot.domain_x;
  const auto& domain_y = plot.domain_y;

  if (series.xs.size() != series.ys.size()) {
    // FIXME error msg
    return ERROR_INVALID_ARGUMENT;
  }

  // draw line
  {
    Path path;
    for (size_t i = 0; i < series.xs.size(); ++i) {
      auto x = series.xs[i];
      auto y = series.ys[i];
      auto sx = clip.x + domain_translate(domain_x, x) * clip.w;
      auto sy = clip.y + (1.0 - domain_translate(domain_y, y)) * clip.h;

      if (i == 0) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    StrokeStyle style;
    style.line_width = series.line_width;
    style.colour = series.line_colour;
    strokePath(layer, clip, path, style);
  }

  // draw points
  auto point_size = to_px(layer->measures, series.point_size);
  if (point_size > 0) {
    FillStyle style;
    style.colour = series.point_colour;
    for (size_t i = 0; i < series.xs.size(); ++i) {
      auto x = series.xs[i];
      auto y = series.ys[i];
      auto sx = clip.x + domain_translate(domain_x, x) * clip.w;
      auto sy = clip.y + (1.0 - domain_translate(domain_y, y)) * clip.h;

      // FIXME point style
      Path path;
      path.moveTo(sx + point_size, sy);
      path.arcTo(sx, sy, point_size, 0, M_PI * 2);
      fillPath(layer, clip, path, style);
    }
  }

  return OK;
}

ReturnCode configure(const plist::Property& prop, PlotConfig* config) {
  if (!prop.child) {
    return ERROR_INVALID_ARGUMENT;
  }

  PlotLinesConfig series;
  static const ParserDefinitions pdefs = {
    {"xs", std::bind(&parseDataSeries, std::placeholders::_1, &series.xs)},
    {"ys", std::bind(&parseDataSeries, std::placeholders::_1, &series.ys)},
    {
      "colour",
      configure_multiprop({
          std::bind(&configure_colour, std::placeholders::_1, &series.line_colour),
          std::bind(&configure_colour, std::placeholders::_1, &series.point_colour),
      })
    },
    {"line-colour", std::bind(&configure_colour, std::placeholders::_1, &series.line_colour)},
    {"line-width", std::bind(&parseMeasureProp, std::placeholders::_1, &series.line_width)},
    {"point-colour", std::bind(&configure_colour, std::placeholders::_1, &series.point_colour)},
    {"point-size", std::bind(&parseMeasureProp, std::placeholders::_1, &series.point_size)},
  };

  if (auto rc = parseAll(*prop.child, pdefs); !rc) {
    return rc;
  }

  domain_fit(series.xs, &config->domain_x);
  domain_fit(series.ys, &config->domain_y);

  config->series.emplace_back(PlotSeries {
    .draw = std::bind(
        &draw_lines,
        std::placeholders::_1,
        series,
        std::placeholders::_2,
        std::placeholders::_3),
  });

  return OK;
}

} // namespace lines
} // namespace plot
} // namespace plotfx


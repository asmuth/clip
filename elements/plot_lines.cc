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

static const double kDefaultLineWidthPT = 2;

ReturnCode plot_lines_draw(
    const PlotLinesConfig& config,
    const Rectangle& clip,
    Layer* layer) {
  for (const auto& group : config.groups) {
    Path path;
    for (size_t i = group.begin; i < group.end; ++i) {
      auto sx = clip.x + config.x[i] * clip.w;
      auto sy = clip.y + (1.0 - config.y[i]) * clip.h;

      if (i == group.begin) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    StrokeStyle style;
    style.line_width = config.line_width;
    style.color = config.colors.empty()
        ? Color{}
        : config.colors[group.begin % config.colors.size()];

    strokePath(layer, clip, path, style);
  }

  return OK;
}

ReturnCode plot_lines_bind(
    PlotLinesConfig& config,
    ElementRef* elem) {
  auto e = std::make_unique<Element>();
  e->draw = std::bind(
      &plot_lines_draw,
      config,
      std::placeholders::_1,
      std::placeholders::_2);

  *elem = std::move(e);
  return OK;
}

ReturnCode plot_lines_configure(
    const Document& doc,
    const plist::PropertyList& plist,
    const DomainMap& scales,
    ElementRef* elem) {
  SeriesRef data_x;
  SeriesRef data_y;
  SeriesRef data_group;

  std::string scale_x = SCALE_DEFAULT_X;
  std::string scale_y = SCALE_DEFAULT_Y;

  Variable<Color> color;
  DomainConfig color_domain;
  ColorScheme color_palette;
  Measure line_width;

  static const ParserDefinitions pdefs = {
    {"x", configure_series_fn(&data_x)},
    {"x-scale", std::bind(&configure_string, std::placeholders::_1, &scale_x)},
    {"y", configure_series_fn(&data_y)},
    {"y-scale", std::bind(&configure_string, std::placeholders::_1, &scale_y)},
    {"group", configure_series_fn(&data_group)},
    {"color", configure_var(&color, configure_color_fn())},
    {"width", std::bind(&configure_measure_rel, std::placeholders::_1, doc.dpi, doc.font_size, &line_width)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  /* check dataset */
  if (!data_x || !data_y) {
    return ERROR_INVALID_ARGUMENT;
  }

  if ((data_x->size() != data_y->size()) ||
      (data_x->size() != data_group->size())) {
    return ERROR_INVALID_ARGUMENT;
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

  /* load data */
  PlotLinesConfig config;
  config.x = domain_translate(*domain_x, *data_x);
  config.y = domain_translate(*domain_y, *data_y);
  config.line_width = measure_or(line_width, from_pt(kDefaultLineWidthPT, doc.dpi));
  config.colors = resolve(color, series_to_colors(color_domain, color_palette));

  /* group data */
  if (data_group) {
    if (data_x->size() != data_group->size()) {
      return ERROR_INVALID_ARGUMENT;
    }

    config.groups = plotfx::series_group(*data_group);
  } else {
    DataGroup g;
    g.begin = 0;
    g.end = data_x->size();
    config.groups.emplace_back(g);
  }

  return plot_lines_bind(config, elem);
}

} // namespace plot
} // namespace plotfx


/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include "gridlines.h"
#include "document.h"
#include "utils/algo.h"

using namespace std::placeholders;

namespace plotfx {
namespace gridlines {

static const double kDefaultLineWidthPT = 1;

ReturnCode draw(
    const GridlineDefinition& grid,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& bbox = layout.content_box;

  StrokeStyle style;
  style.line_width = grid.line_width;
  style.color = grid.line_color;

  std::vector<double> ticks_horiz;
  if (auto rc = grid.layout_x(grid.scale_x, &ticks_horiz); !rc) {
    return rc;
  }

  for (const auto& tick : ticks_horiz) {
    auto line_y = bbox.y + bbox.h * (1.0 - tick);

    strokeLine(
        layer,
        Point(bbox.x, line_y),
        Point(bbox.x + bbox.w, line_y),
        style);
  }

  std::vector<double> ticks_vert;
  if (auto rc = grid.layout_y(grid.scale_y, &ticks_vert); !rc) {
    return rc;
  }

  for (const auto& tick : ticks_vert) {
    auto line_x = bbox.x + bbox.w * tick;

    strokeLine(
        layer,
        Point(line_x, bbox.y),
        Point(line_x, bbox.y + bbox.h),
        style);
  }

  return OK;
}

ReturnCode grid_layout_geom(
    const DomainConfig& domain,
    std::vector<double>* ticks) {
  uint32_t num_ticks = 8; // FIXME make configurable
  double min = domain.min.value_or(0.0f);
  double max = domain.max.value_or(0.0f);

  ticks->clear();

  for (size_t i = 0; i < num_ticks; ++i) {
    ticks->emplace_back((1.0f / (num_ticks - 1)) * i);
  }

  return OK;
}

ReturnCode configure_grid_layout(
    const plist::Property& prop,
    GridlineLayout* layout) {
  if (plist::is_value_literal(prop, "geom")) {
    *layout = bind(&grid_layout_geom, _1, _2);
    return OK;
  }

  return ReturnCode::errorf(
      "EARG",
      "invalid value for 'grid': $0; expected one of: geom(...)",
      prop.value);
}

ReturnCode layout(
    const GridlineDefinition& config,
    const Layer& layer,
    LayoutInfo* layout) {
  /* nothing to do */
  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data,
    const Document& doc,
    const Environment& env,
    GridlineDefinition* config) {
  /* set defaults from environment */
  config->scale_x = env.scale_x;
  config->scale_y = env.scale_y;
  config->layout_x = bind(&grid_layout_geom, _1, _2);
  config->layout_y = bind(&grid_layout_geom, _1, _2);
  config->line_width = from_pt(1);
  config->line_color = Color::fromRGB(.9, .9, .9); // TODO

  /* parse properties */
  static const ParserDefinitions pdefs = {
    {"scale-x", bind(&domain_configure, _1, &config->scale_x)},
    {"scale-x-min", bind(&configure_float_opt, _1, &config->scale_x.min)},
    {"scale-x-max", bind(&configure_float_opt, _1, &config->scale_x.max)},
    {"scale-x-padding", bind(&configure_float, _1, &config->scale_x.padding)},
    {"scale-y", bind(&domain_configure, _1, &config->scale_y)},
    {"scale-y-min", bind(&configure_float_opt, _1, &config->scale_y.min)},
    {"scale-y-max", bind(&configure_float_opt, _1, &config->scale_y.max)},
    {"scale-y-padding", bind(&configure_float, _1, &config->scale_y.padding)},
    {"layout-x", bind(&configure_grid_layout, _1, &config->layout_x)},
    {"stroke", bind(&configure_measure, _1, &config->line_width)},
    {"color", bind(&configure_color, _1, &config->line_color)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  return OK;
}

} // namespace gridlines
} // namespace plotfx


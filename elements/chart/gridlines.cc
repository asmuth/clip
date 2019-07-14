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
#include "gridlines.h"
#include "core/environment.h"
#include "core/layout.h"
#include "utils/algo.h"

using namespace std::placeholders;

namespace fviz {
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

  ScaleLayout slayout_x;
  grid.layout_x(grid.scale_x, &slayout_x);

  ScaleLayout slayout_y;
  grid.layout_y(grid.scale_y, &slayout_y);

  for (const auto& tick : slayout_x.ticks) {
    auto line_x = bbox.x + bbox.w * tick;

    strokeLine(
        layer,
        Point(line_x, bbox.y),
        Point(line_x, bbox.y + bbox.h),
        style);
  }

  for (const auto& tick : slayout_y.ticks) {
    auto line_y = bbox.y + bbox.h * (1.0 - tick);

    strokeLine(
        layer,
        Point(bbox.x, line_y),
        Point(bbox.x + bbox.w, line_y),
        style);
  }

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    GridlineDefinition* config) {
  /* set defaults from environment */
  config->scale_x = env.scale_x;
  config->scale_y = env.scale_y;
  config->layout_x = env.scale_layout_x;
  config->layout_y = env.scale_layout_y;
  config->line_width = from_pt(1);
  config->line_color = Color::fromRGB(.9, .9, .9); // TODO

  /* parse properties */
  ParserDefinitions pdefs = {
    {"scale-x", bind(&domain_configure, _1, &config->scale_x)},
    {"scale-x-min", bind(&configure_float_opt, _1, &config->scale_x.min)},
    {"scale-x-max", bind(&configure_float_opt, _1, &config->scale_x.max)},
    {"scale-x-padding", bind(&configure_float, _1, &config->scale_x.padding)},
    {"scale-y", bind(&domain_configure, _1, &config->scale_y)},
    {"scale-y-min", bind(&configure_float_opt, _1, &config->scale_y.min)},
    {"scale-y-max", bind(&configure_float_opt, _1, &config->scale_y.max)},
    {"scale-y-padding", bind(&configure_float, _1, &config->scale_y.padding)},
    {"layout-x", bind(&configure_scale_layout, _1, &config->layout_x)},
    {"layout-y", bind(&configure_scale_layout, _1, &config->layout_y)},
    {"stroke", bind(&configure_measure, _1, &config->line_width)},
    {"color", bind(&configure_color, _1, &config->line_color)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  return OK;
}

} // namespace gridlines
} // namespace fviz


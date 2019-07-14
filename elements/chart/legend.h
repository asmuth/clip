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
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include "graphics/layer.h"
#include "core/environment.h"
#include "core/layout.h"

namespace fviz {

const std::string LEGEND_DEFAULT = "default";

enum class LegendPlacement {
  OFF,
  INSIDE,
  OUTSIDE
};

using LegendItemList = std::vector<std::string>;

struct LegendConfig {
  LegendConfig();
  std::string key;
  Color text_color;
  Color border_color;
  FontInfo font;
  Measure padding_horiz;
  Measure padding_vert;
  Measure padding_item_horiz;
  Measure padding_item_vert;
  Measure margins[4];
  Measure item_margins[4];
  LegendPlacement placement;
  HAlign position_horiz;
  VAlign position_vert;
  std::string title;
  std::vector<std::string> items;
  std::vector<Color> colors;
  LayoutSettings layout;
};

namespace legend {

ReturnCode draw(
    const LegendConfig& config,
    const LayoutInfo& layout,
    Layer* layer);

ReturnCode reflow(
    const LegendConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    LegendConfig* config);


} // namespace legned
} // namespace fviz


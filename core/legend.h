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
#include "graphics/layout.h"
#include "source/document.h"

namespace fviz {

const std::string LEGEND_DEFAULT = "default";

enum class LegendPlacement {
  OFF,
  INSIDE,
  OUTSIDE
};

struct LegendItem {
  std::string title;
  Color color;
};

struct LegendItemGroup {
  std::vector<LegendItem> items;
  std::string title;
};

using LegendItemList = std::vector<LegendItem>;
using LegendItemMap = std::unordered_map<std::string, std::vector<LegendItemGroup>>;

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
  std::vector<LegendItemGroup> groups;
  LayoutSettings layout;
};

namespace legend {

ReturnCode draw(
    const LegendConfig& config,
    const LayoutState& clip,
    Layer* layer);

ReturnCode layout(
    const LegendConfig& config,
    const Layer& layer,
    LayoutState* layout);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    LegendConfig* config);


} // namespace legned
} // namespace fviz


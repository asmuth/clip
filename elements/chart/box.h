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
#include "sexpr.h"
#include "core/environment.h"
#include "element.h"
#include "core/layout.h"

namespace fviz {
namespace box {

struct BoxBorderConfig {
  Color color;
  Measure width;
};

struct BoxConfig {
  std::vector<ElementRef> children;
  FontInfo font;
  Measure font_size;
  ColorScheme color_scheme;
  std::optional<Color> background;
  Color text_color;
  Color border_color;
  std::array<Measure, 4> margins;
  std::array<BoxBorderConfig, 4> borders;
  LayoutSettings layout;
};

ReturnCode draw(
    const BoxConfig& config,
    const LayoutInfo& layout,
    Layer* layer);

ReturnCode reflow(
    const BoxConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    BoxConfig* config);

} // namespace box
} // namespace fviz


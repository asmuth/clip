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
#include <stdlib.h>
#include <sexpr.h>
#include <graphics/layer.h>
#include <source/core/scale.h>
#include <source/element.h>
#include <source/config_helpers.h>

namespace fviz {
namespace plot {
namespace labels {

struct PlotLabelsConfig {
  std::vector<double> x;
  std::vector<double> y;
  std::vector<std::string> labels;
  FontInfo label_font;
  Measure label_padding;
  Measure label_font_size;
  Color label_color;
};

ReturnCode draw(
    const PlotLabelsConfig& config,
    const Rectangle& clip,
    Layer* layer);;

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data,
    const Document& doc,
    PlotLabelsConfig* config);

} // namespace labels
} // namespace plot
} // namespace fviz


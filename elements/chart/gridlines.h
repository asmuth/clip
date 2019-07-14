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
#include <core/scale.h>
#include <element.h>
#include <config_helpers.h>

namespace fviz {
namespace gridlines {

using GridlineLayout = std::function<ReturnCode (
    const DomainConfig& domain,
    std::vector<double>*)>;

struct GridlineDefinition {
  DomainConfig scale_x;
  DomainConfig scale_y;
  ScaleLayoutFn layout_x;
  ScaleLayoutFn layout_y;
  Measure line_width;
  Color line_color;
  LayoutSettings layout;
};

ReturnCode draw(
    const GridlineDefinition& config,
    const LayoutInfo& layout,
    Layer* layer);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    GridlineDefinition* config);

} // namespace gridlines
} // namespace fviz


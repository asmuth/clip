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
#include "return_code.h"
#include "sexpr.h"
#include "graphics/path.h"
#include "graphics/color.h"
#include "graphics/measure.h"

namespace fviz {
struct Layer;

/**
 * The FillStyle controls how shapes are filled in
 */
using FillStyle = std::function<
    ReturnCode (
        const Polygon2& poly,
        Layer* layer)>;

/**
 * Solid color fill
 */
FillStyle fill_style_solid(const Color& color);

/**
 * 'Hatched' fill
 */
FillStyle fill_style_hatch(
    const Color& color,
    double angle_deg,
    double offset,
    double stride,
    double width);

/**
 * No fill
 */
FillStyle fill_style_none();


/**
 * The StrokeStyle controls how shapes are stroked
 */
struct StrokeStyle {
  enum DashType {
    SOLID,
    DASH
  };

  StrokeStyle() :
    color(Color::fromRGB(0, 0, 0)),
    dash_type(SOLID) {}

  Measure line_width;
  Color color;
  DashType dash_type;
  Measure dash_offset;
  std::vector<Measure> dash_pattern;
};



} // namespace fviz


/**
 * This file is part of the "clip" project
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
#include "utils/option.h"

namespace clip {
struct Page;

enum class AntialiasingMode {
  ENABLE, DISABLE
};

/**
 * The FillStyle controls how shapes are filled in
 */
struct FillStyle {
  FillStyle();

  Option<Color> color;
  bool hatch;
  double hatch_angle_deg;
  double hatch_offset;
  double hatch_stride;
  double hatch_width;
};

/**
 * The StrokeStyle controls how shapes are stroked
 */
struct StrokeStyle {
  enum DashType {
    SOLID,
    DASH
  };

  StrokeStyle();

  Number line_width;
  Color color;
  DashType dash_type;
  Measure dash_offset;
  std::vector<Measure> dash_pattern;
};

namespace draw_style {

/**
 * Fill the shape with a solid color
 */
struct fill_solid {
  fill_solid(const Color& c) : color(c) {}
  Color color;
};

/**
 * Fill the shape with a hatch pattern
 */
struct fill_hatch {
  Color color;
  double angle_deg;
  Number offset;
  Number stride;
  Number width;
};

/**
 * Stroke the outline of the shape with a solid line
 */
struct stroke_solid {
  Color color;
  Number width;
};

/**
 * Stroke the outline of the shape with a dotted line
 */
struct stroke_dash {
  Color color;
  Number width;
  std::vector<Number> pattern;
  Number offset;
};

/**
 * Draw the shape using multiple styles. The order in which styles are applied
 * is as follows (first to last):
 *
 *   - fill_solid
 *   - fill_hatch
 *   - stroke_solid
 *   - stroke_dash
 *
 */
struct compound {
  std::vector<draw_style::fill_solid> fill_solid;
  std::vector<draw_style::fill_hatch> fill_hatch;
  std::vector<draw_style::stroke_solid> stroke_solid;
  std::vector<draw_style::stroke_dash> stroke_dash;
};


} // namespace draw_style
} // namespace clip


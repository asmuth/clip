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
#include <stdlib.h>
#include <string>
#include <variant>
#include <vector>

#include "color.h"
#include "text.h"
#include "text_layout.h"
#include "measure.h"
#include "font_lookup.h"
#include "style.h"

namespace clip::draw_cmd {

struct Text {
  std::string text;
  std::vector<text::GlyphPlacementGroup> glyphs;

  // The `origin` refers to the start of the baseline
  Point origin;

  TextStyle style;
  Option<mat3> transform;
};

struct Shape {
  Path path;
  StrokeStyle stroke_style;
  FillStyle fill_style;
  Option<AntialiasingMode> antialiasing_mode;
};

struct Polygon {
  Poly2 poly;
  StrokeStyle stroke_style;
  FillStyle fill_style;
  Option<AntialiasingMode> antialiasing_mode;
};

} // namespace clip::draw_cmd


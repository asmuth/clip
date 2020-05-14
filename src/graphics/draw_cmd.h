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
#include <vector>

#include "color.h"
#include "text.h"
#include "text_layout.h"
#include "measure.h"
#include "font_lookup.h"
#include "style.h"

namespace clip {

struct DrawCommand {
  Path path;
  StrokeStyle stroke_style;
  FillStyle fill_style;
  Option<AntialiasingMode> antialiasing_mode;
};

/**
 * The draw command list represents a list of abstract 2D vector graphics
 * operations, such as rendering text and drawing polygons. Note that the "list"
 * is not necessarily a flat list, but may be a tree.
 */
using DrawCommandList = std::vector<DrawCommand>;

struct TextInfo {
  std::string text;
  std::vector<text::GlyphPlacementGroup> glyphs;

  // The `origin` refers to the start of the baseline
  Point origin;

  TextStyle style;
  Option<mat3> transform;
};

} // namespace clip


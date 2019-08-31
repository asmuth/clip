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
#include <vector>
#include <string>

#include "color.h"
#include "text.h"
#include "text_layout.h"
#include "measure.h"
#include "font_lookup.h"
#include "style.h"

namespace clip {

enum class AntialiasingMode {
  ENABLE, DISABLE
};

struct PageTextElement {
  std::string text;
  std::vector<text::GlyphPlacementGroup> glyphs;

  // The `origin` refers to the start of the baseline
  Point origin;

  TextStyle style;
  std::optional<uint32_t> zindex;
  Rectangle clip;
  std::optional<mat3> transform;
};

struct PageShapeElement {
  Path path;
  StrokeStyle stroke_style;
  std::optional<Color> fill_color;
  std::optional<uint32_t> zindex;
  Rectangle clip;
  std::optional<AntialiasingMode> antialiasing_mode;
};

} // namespace clip


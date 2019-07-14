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
#include "fviz.h"
#include "path.h"
#include "color.h"
#include "measure.h"
#include "layout.h"

namespace fviz {
class Layer;

enum class TextDirection {
  LTR, RTL
};

struct FontInfo {
  std::string font_file;
  std::string font_family_css;
};

struct TextStyle {
  TextStyle();
  TextDirection direction;
  FontInfo font;
  Measure font_size;
  Color color;
};

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    const TextStyle& text_style,
    Layer* layer);

} // namespace fviz


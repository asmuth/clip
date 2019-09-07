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
#include <graphics/text.h>
#include <graphics/text_shaper.h>
#include "graphics/text_layout.h"
#include "graphics/text_support.h"
#include <graphics/page_description.h>
#include <graphics/brush.h>

namespace clip {

TextStyle::TextStyle() :
    direction(TextDirection::LTR) {}

Status text_measure_label(
    const std::string& text,
    TextDirection text_direction_base,
    const FontInfo& font,
    double font_size,
    double dpi,
    Rectangle* bbox) {
  text::TextSpan span;
  span.text = text;
  span.text_direction = text_direction_base;
  span.font = font;
  span.font_size = font_size;
  span.span_id = 0;

  text::TextLine line;
  line.spans.push_back(span);
  line.base_direction = text_direction_base;

  return text_layout_line(line, dpi, nullptr, bbox);
}

} // namespace clip


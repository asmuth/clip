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
#include "fviz.h"
#include "graphics/text_layout.h"
#include "graphics/text_shaper.h"

namespace fviz {
namespace text {

Status text_layout_ltr(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    std::vector<GlyphPlacement>* glyphs,
    Rectangle* bbox) {
  double line_length = 0.0f;
  double top = 0.0f;
  double bottom = 0.0f;

  auto rc = shaper->shapeText(
      text,
      font_info,
      font_size,
      dpi,
      [&] (const GlyphInfo& gi) {
        GlyphPlacement gp = {
          .codepoint = gi.codepoint,
          .x = line_length,
          .y = 0
        };

        if (glyphs) {
          glyphs->emplace_back(gp);
        }

        line_length += gi.advance_x;
        top = std::min(-gi.metrics_ascender, top);
        bottom = std::max(-gi.metrics_descender, bottom);
      });

  if (rc != OK) {
    return rc;
  }

  *bbox = Rectangle(
      0,
      top,
      line_length,
      bottom - top);

  return OK;
}


Status text_layout(
    const std::string& text,
    const FontInfo& font,
    double font_size,
    double dpi,
    TextDirection direction,
    const TextShaper* shaper,
    std::vector<GlyphPlacement>* glyphs,
    Rectangle* bbox) {
  switch (direction) {
    case TextDirection::LTR:
      return text_layout_ltr(text, font, font_size, dpi, shaper, glyphs, bbox);
    default:
      return ERROR;
  }
}

Status text_measure_span(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    Rectangle* bbox) {
  return text_layout_ltr(text, font_info, font_size, dpi, shaper, nullptr, bbox);
}

} // namespace text
} // namespace fviz


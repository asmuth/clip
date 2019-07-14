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

Status text_measure_span(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    Rectangle* rect) {
  double line_length = 0;
  double top = 0.0f;
  double bottom = 0.0f;

  std::vector<GlyphInfo> glyphs;
  auto rc = shaper->shapeText(
      text,
      font_info,
      font_size,
      dpi,
      [&] (const GlyphInfo& gi) {
        line_length += gi.advance_x;
        top = std::min(-gi.metrics_ascender, top);
        bottom = std::max(-gi.metrics_descender, bottom);
        glyphs.emplace_back(gi);
      });

  if (rc != OK) {
    return rc;
  }

  *rect = Rectangle(0, top, line_length, bottom - top);
  return OK;
}

Status layoutTextLTR(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb) {
  double line_length;
  std::vector<GlyphInfo> glyphs;
  auto rc = shaper->shapeText(
      text,
      font_info,
      font_size,
      dpi,
      [&] (const GlyphInfo& gi) {
        line_length += gi.advance_x;
        glyphs.emplace_back(gi);
      });

  if (rc != OK) {
    return rc;
  }

  auto gx = x;
  auto gy = y;


  for (const auto& gi : glyphs) {
    double baseline_offset = 0;

    GlyphPlacement g;
    glyph_cb(GlyphPlacement {
      .codepoint = gi.codepoint,
      .x = gx,
      .y = gy + baseline_offset
    });

    gx += gi.advance_x;
  }

  return OK;
}

Status layoutText(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font,
    double font_size,
    double dpi,
    TextDirection direction,
    const TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb) {
  switch (direction) {
    case TextDirection::LTR:
      return layoutTextLTR(text, x, y, font, font_size, dpi, shaper, glyph_cb);
    case TextDirection::RTL:
      return ERROR;
    default:
      return ERROR;
  }
}


} // namespace text
} // namespace fviz


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

Status text_layout_hspan(
    const std::string& text,
    const TextDirection text_direction,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphSpan>* glyph_spans,
    Rectangle* bbox) {
  std::vector<GlyphInfo> glyph_list;
  auto shaping_rc = text_shape_with_font_fallback(
      text,
      font_info,
      font_size,
      dpi,
      &glyph_list);

  if (!shaping_rc) {
    return shaping_rc;
  }

  double line_length = 0.0f;
  double line_top = 0.0f;
  double line_bottom = 0.0f;
  for (const auto& gi : glyph_list) {
    GlyphPlacement gp = {
      .codepoint = gi.codepoint,
      .x = line_length,
      .y = 0
    };

    GlyphSpan span;
    span.font = gi.font;
    span.glyphs.push_back(gp);

    // TODO merge glyph spans witht the same font
    if (glyph_spans) {
      glyph_spans->emplace_back(span);
    }

    line_length += gi.advance_x;
    line_top = std::min(-gi.metrics_ascender, line_top);
    line_bottom = std::max(-gi.metrics_descender, line_bottom);
  }

  *bbox = Rectangle(
      0,
      line_top,
      line_length,
      line_bottom - line_top);

  return OK;
}

Status text_measure_span(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    Rectangle* bbox) {
  return text_layout_hspan(
      text,
      TextDirection::LTR,
      font_info,
      font_size,
      dpi,
      nullptr,
      bbox);
}

} // namespace text
} // namespace fviz


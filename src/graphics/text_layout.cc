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
#include "graphics/text_backend.h"
#include "graphics/text_layout.h"
#include "graphics/text_shaper.h"

#include <numeric>

namespace clip {
namespace text {

Status text_layout_span(
    const TextSpan& span,
    double dpi,
    std::vector<GlyphPlacement>* glyphs,
    double* span_length,
    double* span_top,
    double* span_bottom) {
  std::vector<GlyphInfo> glyph_list;
  auto shaping_rc = text_shape_run_with_font_fallback(
      span.text,
      span.text_direction,
      span.language,
      span.script,
      span.font,
      unit_to_pt(span.font_size, dpi),
      dpi,
      &glyph_list);

  if (!shaping_rc) {
    return shaping_rc;
  }

  for (const auto& gi : glyph_list) {
    GlyphPlacement gp;
    gp.font = gi.font;
    gp.codepoint = gi.codepoint;
    gp.y = 0;
    gp.x = *span_length;
    gp.span_id = span.span_id;
    glyphs->emplace_back(gp);

    *span_length += gi.advance_x;
    *span_top = std::max(gi.metrics_ascender, *span_top);
    *span_bottom = std::min(gi.metrics_descender, *span_bottom);
  }

  return OK;
}

Status text_layout_line(
    const TextLine& text_line,
    double dpi,
    std::vector<GlyphPlacementGroup>* glyphs,
    Rectangle* bbox) {
  double line_top = 0.0;
  double line_bottom = 0.0;
  double line_length = 0;
  for (const auto& span : text_line.spans) {
    double span_length = 0.0;
    std::vector<GlyphPlacement> span_glyphs;
    auto rc = text_layout_span(
        span,
        dpi,
        &span_glyphs,
        &span_length,
        &line_top,
        &line_bottom);

    if (rc != OK) {
      return rc;
    }

    for (auto& gi : span_glyphs) {
      switch (text_line.base_direction) {
        case TextDirection::LTR:
          gi.x += line_length;
          break;
        case TextDirection::RTL:
          gi.x -= line_length;
          gi.x -= span_length;
          break;
      }

      // TODO merge glyph spans with the same font
      GlyphPlacementGroup gg;
      gg.font = gi.font;
      gg.glyphs.emplace_back(gi);

      if (glyphs) {
        glyphs->emplace_back(gg);
      }
    }

    line_length += span_length;
  }

  double line_left = 0.0;
  switch (text_line.base_direction) {
    case TextDirection::LTR:
      line_left = 0;
      break;
    case TextDirection::RTL:
      line_left = -line_length;
      break;
  }

  *bbox = Rectangle(
      line_left,
      line_bottom,
      line_length,
      line_top - line_bottom);

  return OK;
}

} // namespace text
} // namespace clip


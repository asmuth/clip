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
#include "graphics/text_backend.h"
#include "graphics/text_layout.h"
#include "graphics/text_shaper.h"

namespace fviz {
namespace text {

Status text_layout_hspan(
    const std::string& text_logical,
    const TextDirection text_direction,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphSpan>* glyph_spans,
    Rectangle* bbox) {
  TextSpan text_span;
  text_analyze_bidi_span(text_logical, text_direction, &text_span);

  for (size_t i = 0; i < text_span.text_runs.size(); ++i) {
    std::string text_run_dir;
    switch (text_span.text_directions[i]) {
      case TextDirection::LTR:
        text_run_dir = "ltr";
        break;
      case TextDirection::RTL:
        text_run_dir = "rtl";
        break;
    }

    std::cerr
        << "t=" << text_span.text_runs[i]
        << " "
        << "d=" << text_run_dir
        << std::endl;
  }

  std::vector<GlyphInfo> glyph_list;
  auto shaping_rc = text_shape_run_with_font_fallback(
      text_logical,
      text_direction,
      font_info,
      font_size,
      dpi,
      &glyph_list);

  if (!shaping_rc) {
    return shaping_rc;
  }

  double span_length = 0.0;
  double span_top = 0.0;
  double span_bottom = 0.0;
  for (const auto& gi : glyph_list) {
    GlyphPlacement gp;
    gp.codepoint = gi.codepoint;
    gp.y = 0;

    switch (text_direction) {
      case TextDirection::LTR:
        gp.x = span_length;
        break;
      case TextDirection::RTL:
        gp.x = -span_length - gi.advance_x;
        break;
    }

    GlyphSpan span;
    span.font = gi.font;
    span.glyphs.push_back(gp);

    // TODO merge glyph spans witht the same font
    if (glyph_spans) {
      glyph_spans->emplace_back(span);
    }

    span_length += gi.advance_x;
    span_top = std::min(-gi.metrics_ascender, span_top);
    span_bottom = std::max(-gi.metrics_descender, span_bottom);
  }


  double span_left = 0.0;
  switch (text_direction) {
    case TextDirection::LTR:
      span_left = 0;
      break;
    case TextDirection::RTL:
      span_left = -span_length;
      break;
  }

  *bbox = Rectangle(
      span_left,
      span_top,
      span_length,
      span_bottom - span_top);

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


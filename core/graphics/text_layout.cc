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

#include <boost/locale.hpp>

namespace fviz {
namespace text {

struct TextRun {
  FontInfo font;
  std::vector<GlyphInfo> glyphs;
};

Status text_layout_with_font_fallback(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    std::vector<TextRun>* text_runs) {

  boost::locale::generator locale_gen;
  boost::locale::boundary::ssegment_index grapheme_iter(
        boost::locale::boundary::character,
        text.begin(),
        text.end(),
        locale_gen("en_US.UTF-8"));

  for (const auto& grapheme_str : grapheme_iter) {
    TextRun grapheme;
    grapheme.font = font_info;

    auto rc = shaper->shapeText(
        grapheme_str,
        font_info,
        font_size,
        dpi,
        &grapheme.glyphs);

    if (rc != OK) {
      return rc;
    }

    text_runs->emplace_back(grapheme);
  }

  return OK;
}

Status text_layout_ltr(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    const TextShaper* shaper,
    std::vector<GlyphPlacement>* glyphs,
    Rectangle* bbox) {
  std::vector<TextRun> text_runs;

  auto shaping_rc = text_layout_with_font_fallback(
      text,
      font_info,
      font_size,
      dpi,
      shaper,
      &text_runs);

  if (!shaping_rc) {
    return shaping_rc;
  }

  double line_length = 0.0f;
  double line_top = 0.0f;
  double line_bottom = 0.0f;
  for (const auto& r : text_runs) {
    for (const auto& gi : r.glyphs) {
      GlyphPlacement gp = {
        .codepoint = gi.codepoint,
        .x = line_length,
        .y = 0
      };

      if (glyphs) {
        glyphs->emplace_back(gp);
      }

      line_length += gi.advance_x;
      line_top = std::min(-gi.metrics_ascender, line_top);
      line_bottom = std::max(-gi.metrics_descender, line_bottom);
    }
  }

  *bbox = Rectangle(
      0,
      line_top,
      line_length,
      line_bottom - line_top);

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


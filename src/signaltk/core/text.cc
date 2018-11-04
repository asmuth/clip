/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <signaltk/core/text.h>
#include <signaltk/text/text_shaper.h>
#include <signaltk/text/text_layout.h>
#include <signaltk/core/layer.h>

namespace signaltk {

TextStyle::TextStyle() :
    direction(TextDirection::LTR),
    halign(TextHAlign::LEFT),
    valign(TextVAlign::BASELINE),
    font_size(12) {}

Status drawText(
    const std::string& text,
    double x,
    double y,
    const TextStyle& text_style,
    Layer* layer) {
  FontInfo font_info {
    .font_file = "/Library/Fonts/Arial.ttf",
    .font_size = 12
  };

  std::vector<GlyphPlacement> glyphs;
  auto rc = text::layoutText(
      text,
      x,
      y,
      font_info,
      text_style.direction,
      text_style.halign,
      text_style.valign,
      &layer->text_shaper,
      [&glyphs] (const GlyphPlacement& g) { glyphs.emplace_back(g); });

  if (rc != OK) {
    return rc;
  }

  return drawTextGlyphs(font_info, glyphs.data(), glyphs.size(), layer);
}

Status drawTextGlyphs(
    const FontInfo& font_info,
    const GlyphPlacement* glyphs,
    size_t glyph_count,
    Layer* layer) {
  return layer->rasterizer.drawTextGlyphs(font_info, glyphs, glyph_count);
}

} // namespace signaltk


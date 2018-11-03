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
#include <signaltk/core/layer.h>

namespace signaltk {

TextStyle::TextStyle() :
    halign(TextHAlign::LEFT),
    valign(TextVAlign::CENTER),
    font_size(32) {}

Status drawText(
    const std::string& text,
    const TextStyle& text_style,
    double x,
    double y,
    Layer* layer) {
  auto ctx = layer->rasterizer.ctx;
  auto dpi = layer->dpi;

  FontInfo font_info {
    .font_file = "/Library/Fonts/Arial.ttf",
    .font_size = 42
  };

  std::vector<GlyphPlacement> glyphs;
  auto rc = layer->text_shaper.shapeText(
      text,
      font_info,
      [&glyphs] (const text::TextShaper::GlyphPlacement& gi) {
         GlyphPlacement g;
         glyphs.emplace_back(GlyphPlacement {
           .codepoint = gi.codepoint,
           .x = 100,
           .y = 100
         });
      });

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


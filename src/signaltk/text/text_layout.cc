/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <signaltk.h>
#include <signaltk/text/text_layout.h>

namespace signaltk {
namespace text {

Status layoutTextLTR(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font_info,
    TextHAlign halign,
    TextVAlign valign,
    TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb) {
  double line_length;
  std::vector<GlyphInfo> glyphs;
  auto rc = shaper->shapeText(
      text,
      font_info,
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
    GlyphPlacement g;
    glyph_cb(GlyphPlacement {
      .codepoint = gi.codepoint,
      .x = gx,
      .y = gy
    });

    gx += gi.advance_x;
  }

  return OK;
}

Status layoutText(
    const std::string& text,
    double x,
    double y,
    const FontInfo& font_info,
    TextDirection direction,
    TextHAlign halign,
    TextVAlign valign,
    TextShaper* shaper,
    std::function<void (const GlyphPlacement&)> glyph_cb) {
  switch (direction) {
    case TextDirection::LTR:
      return layoutTextLTR(text, x, y, font_info, halign, valign, shaper, glyph_cb);
    case TextDirection::RTL:
      return ERROR_NOT_IMPLEMENTED;
    default:
      return ERROR_INVALID_ARGUMENT;
  }
}


} // namespace text
} // namespace signaltk


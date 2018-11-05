/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "signaltk.h"
#include "graphics/text_layout.h"

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

  switch (halign) {
    case TextHAlign::LEFT:
      break;
    case TextHAlign::CENTER:
      gx -= line_length / 2;
      break;
    case TextHAlign::RIGHT:
      gx -= line_length;
      break;
  }

  for (const auto& gi : glyphs) {
    // FIXME: this is constant for every glpyh in the same font
    double baseline_offset = 0;

    switch (valign) {
      case TextVAlign::BASELINE:
        break;
      case TextVAlign::TOP:
        baseline_offset = gi.metrics_ascender;
        break;
      case TextVAlign::MIDDLE:
        baseline_offset = gi.metrics_ascender - (gi.metrics_ascender + -gi.metrics_descender) / 2;
        break;
      case TextVAlign::BOTTOM:
        baseline_offset = gi.metrics_descender;
        break;
    }

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


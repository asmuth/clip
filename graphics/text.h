/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "signaltk.h"
#include "path.h"

namespace signaltk {
class Layer;

enum class TextDirection {
  LTR, RTL
};

enum class TextHAlign {
  LEFT, CENTER, RIGHT
};

enum class TextVAlign {
  BASELINE, MIDDLE, TOP, BOTTOM
};

struct TextStyle {
  TextStyle();
  TextDirection direction;
  TextHAlign halign;
  TextVAlign valign;
  double font_size;
};

struct FontInfo {
  std::string font_file;
  double font_size;
};

struct GlyphInfo {
  uint32_t codepoint;
  double advance_y;
  double advance_x;
  double metrics_ascender;
  double metrics_descender;
};

struct GlyphPlacement {
  uint32_t codepoint;
  double x;
  double y;
};

Status drawText(
    const std::string& text,
    double x,
    double y,
    const TextStyle& text_style,
    Layer* layer);

Status drawTextGlyphs(
    const FontInfo& font_info,
    const GlyphPlacement* glyphs,
    size_t glyph_count,
    Layer* layer);


} // namespace signaltk


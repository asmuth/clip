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
#include <string>
#include <unordered_map>
#include <functional>

#include <cairo.h>
#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftadvanc.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

#include <signaltk/core/text.h>

namespace signaltk {

class Rasterizer {
public:

  Rasterizer(double width, double height, double dpi);
  ~Rasterizer();
  Rasterizer(const Rasterizer&) = delete;
  Rasterizer& operator=(const Rasterizer&) = delete;

  Status drawTextGlyphs(
      const FontInfo& font_info,
      const GlyphPlacement* glyphs,
      size_t glyph_count);

  double dpi;
  cairo_surface_t* surface;
  cairo_t* ctx;
  FT_Library ft;
  bool ft_ready;
};


} // namespace signaltk


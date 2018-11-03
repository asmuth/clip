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
namespace text {

class TextShaper {
public:

  TextShaper(double dpi);
  ~TextShaper();
  TextShaper(const TextShaper&) = delete;
  TextShaper& operator=(const TextShaper&) = delete;

  Status shapeText(
      const std::string& text,
      const FontInfo& font_info,
      std::function<void (const GlyphInfo&)> glyph_cb);

protected:
  double dpi;
  FT_Library ft;
  bool ft_ready;
  hb_buffer_t* hb_buf;
};

} // namespace text
} // namespace signaltk


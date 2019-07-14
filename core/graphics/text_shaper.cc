/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <graphics/text_shaper.h>
#include <iostream>

namespace fviz {
namespace text {

TextShaper::TextShaper() :
    ft_ready(false),
    hb_buf(hb_buffer_create()) {}

TextShaper::~TextShaper() {
  if (ft_ready) {
    FT_Done_FreeType(ft);
  }
}

Status TextShaper::shapeText(
    const std::string& text,
    const FontInfo& font,
    double font_size,
    double dpi,
    std::function<void (const GlyphInfo&)> glyph_cb) const {
  if (!ft_ready) {
    if (FT_Init_FreeType(&ft)) {
      return ERROR;
    }

    ft_ready = true;
  }

  // FIXME cache
  FT_Face ft_font;
  if (FT_New_Face(ft, font.font_file.c_str(), 0, &ft_font)) {
    return ERROR;
  }

  auto font_size_ft = font_size * (72.0 / dpi) * 64;
  if (FT_Set_Char_Size(ft_font, 0, font_size_ft, dpi, dpi)) {
    FT_Done_Face(ft_font);
    return ERROR;
  }

  auto hb_font = hb_ft_font_create_referenced(ft_font);

  hb_buffer_reset(hb_buf);
  hb_buffer_set_direction(hb_buf, HB_DIRECTION_LTR);
  hb_buffer_set_script(hb_buf, HB_SCRIPT_LATIN);

  hb_buffer_add_utf8(hb_buf, text.data(), text.size(), 0, text.size());
  hb_shape(hb_font, hb_buf, NULL, 0);

  uint32_t glyph_count;
  auto glyph_infos = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
  auto glyph_positions = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);
  for (size_t i = 0; i < glyph_count; ++i) {
    GlyphInfo g;
    g.codepoint = glyph_infos[i].codepoint;
    g.advance_x = glyph_positions[i].x_advance / 64.0;
    g.advance_y = glyph_positions[i].y_advance / 64.0;
    g.metrics_ascender = ft_font->size->metrics.ascender / 64.0; // FIXME this is constant for all glyphs
    g.metrics_descender = ft_font->size->metrics.descender / 64.0; // FIXME this is constant for all glyphs
    glyph_cb(g);
  }

  hb_font_destroy(hb_font);
  FT_Done_Face(ft_font);

  return OK;
}

} // namespace text
} // namespace fviz


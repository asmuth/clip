/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <signaltk/core/rasterize.h>
#include <signaltk/core/image.h>

namespace signaltk {

Rasterizer::Rasterizer(
    Image* pixmap_,
    double dpi_) :
    pixmap(pixmap_),
    dpi(dpi_),
    ft_ready(false) {
  if (!FT_Init_FreeType(&ft)) {
    ft_ready = true;
  }
}

Rasterizer::~Rasterizer() {
  if (ft_ready) {
    FT_Done_FreeType(ft);
  }
}

Status Rasterizer::drawTextGlyphs(
    const FontInfo& font_info,
    const GlyphPlacement* glyphs,
    size_t glyph_count) {
  if (!ft_ready) {
    return ERROR;
  }

  // FIXME cache
  FT_Face ft_font;
  if (FT_New_Face(ft, font_info.font_file.c_str(), 0, &ft_font)) {
    return ERROR;
  }

  if (FT_Set_Char_Size(ft_font, 0, font_info.font_size * 64, dpi, dpi)) {
    FT_Done_Face(ft_font);
    return ERROR;
  }

  for (int i = 0; i < glyph_count; ++i) {
    const auto& g = glyphs[i];

    // FIXME also cache this
    FT_Load_Glyph(ft_font, g.codepoint, FT_LOAD_DEFAULT);
    FT_Render_Glyph(ft_font->glyph, FT_RENDER_MODE_NORMAL);

    const auto& ft_bitmap = ft_font->glyph->bitmap;
    for (uint32_t y = 0; y < ft_bitmap.rows; ++y) {
      for (uint32_t x = 0; x < ft_bitmap.width; ++x) {
        auto v = 1.0 - (ft_bitmap.buffer[y * ft_bitmap.width + x] / 255.0);

        auto ox = x + g.x + ft_font->glyph->bitmap_left;
        auto oy = y + g.y - ft_font->glyph->bitmap_top;

        if (ox >= pixmap->getWidth() || oy >= pixmap->getHeight()) {
          continue;
        }

        // FIXME alpha blend instead
        pixmap->setPixel(ox, oy, Colour::fromRGBA(v, v, v, 1));
      }
    }
  }

  FT_Done_Face(ft_font);
  return OK;
}

} // namespace signaltk


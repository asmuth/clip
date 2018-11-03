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

namespace signaltk {

Rasterizer::Rasterizer(
    double width,
    double height,
    double dpi_) :
    dpi(dpi_),
    ft_ready(false) {
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  ctx = cairo_create(surface);
}

Rasterizer::~Rasterizer() {
  if (ft_ready) {
    FT_Done_FreeType(ft);
  }

  cairo_destroy(ctx);
  cairo_surface_destroy(surface);
}

Status Rasterizer::drawTextGlyphs(
    const FontInfo& font_info,
    const GlyphPlacement* glyphs,
    size_t glyph_count) {
  if (!ft_ready) {
    if (FT_Init_FreeType(&ft)) {
      return ERROR;
    }

    ft_ready = true;
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

  auto cairo_font = cairo_ft_font_face_create_for_ft_face(ft_font, 0);
  auto cairo_glyphs = cairo_glyph_allocate(glyph_count);

  for (int i = 0; i < glyph_count; ++i) {
    printf("render glyph @ %f, %f\n", glyphs[i].x, glyphs[i].y);
    cairo_glyphs[i].index = glyphs[i].codepoint;
    cairo_glyphs[i].x = glyphs[i].x;
    cairo_glyphs[i].y = glyphs[i].y;
  }

  cairo_set_source_rgba(ctx, 0, 0, 0, 1.0);
  cairo_set_font_face(ctx, cairo_font);
  cairo_set_font_size(ctx, font_info.font_size);
  cairo_show_glyphs(ctx, cairo_glyphs, glyph_count);

  cairo_glyph_free(cairo_glyphs);
  FT_Done_Face(ft_font);

  return OK;
}

} // namespace signaltk


/**
 * This file is part of the "plotfx" project
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
#include <iostream>
#include <graphics/rasterize.h>
#include <graphics/image.h>

namespace plotfx {

Rasterizer::Rasterizer(
    uint32_t width,
    uint32_t height,
    double dpi_) :
    dpi(dpi_),
    ft_ready(false) {
  if (!FT_Init_FreeType(&ft)) {
    ft_ready = true;
  }

  cr_surface = cairo_image_surface_create(
      CAIRO_FORMAT_ARGB32,
      width,
      height);

  cr_ctx = cairo_create(cr_surface);
}

Rasterizer::~Rasterizer() {
  if (ft_ready) {
    FT_Done_FreeType(ft);
  }

  cairo_destroy(cr_ctx);
  cairo_surface_destroy(cr_surface);
}

/* rasterize using libcairo */
Status Rasterizer::strokePath(
    const PathData* path_data,
    size_t point_count,
    const StrokeStyle& style) {
  if (point_count < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  cairo_set_source_rgba(
     cr_ctx,
     style.colour.red(),
     style.colour.green(),
     style.colour.blue(),
     style.colour.alpha());

  cairo_set_line_width(cr_ctx, style.line_width);
  for (size_t i = 0; i < point_count; ++i) {
    const auto& cmd = path_data[i];
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        cairo_move_to(cr_ctx, cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        cairo_line_to(cr_ctx, cmd[0], cmd[1]);
        break;
    }
  }

  cairo_stroke(cr_ctx);

  return OK;
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

  auto cairo_face = cairo_ft_font_face_create_for_ft_face(ft_font, 0);
  cairo_set_font_face(cr_ctx, cairo_face);
  cairo_set_font_size(cr_ctx, (font_info.font_size / 72.0) * dpi);

  auto cairo_glyphs = cairo_glyph_allocate(glyph_count);
  for (int i = 0; i < glyph_count; ++i) {
    const auto& g = glyphs[i];
    //FT_Load_Glyph(ft_font, g.codepoint, FT_LOAD_DEFAULT);


    cairo_glyphs[i].index = g.codepoint;
    cairo_glyphs[i].x = g.x;
    cairo_glyphs[i].y = g.y;

    //// render with freetype
    // FT_Render_Glyph(ft_font->glyph, FT_RENDER_MODE_NORMAL);
    // const auto& ft_bitmap = ft_font->glyph->bitmap;
    // for (uint32_t y = 0; y < ft_bitmap.rows; ++y) {
    //   for (uint32_t x = 0; x < ft_bitmap.width; ++x) {
    //     auto v = 1.0 - (ft_bitmap.buffer[y * ft_bitmap.width + x] / 255.0);
    //     auto ox = x + g.x + ft_font->glyph->bitmap_left;
    //     auto oy = y + g.y - ft_font->glyph->bitmap_top;
    //     if (ox >= pixmap->getWidth() || oy >= pixmap->getHeight()) {
    //       continue;
    //     }
    //     pixmap->setPixel(ox, oy, Colour::fromRGBA(v, v, v, 1));
    //   }
    // }
  }

  cairo_show_glyphs(cr_ctx, cairo_glyphs, glyph_count);
  cairo_glyph_free(cairo_glyphs);
  cairo_font_face_destroy(cairo_face);

  FT_Done_Face(ft_font);
  return OK;
}

} // namespace plotfx


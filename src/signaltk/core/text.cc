/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "text.h"
#include <iostream>
#include <cairo-ft.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftadvanc.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

namespace signaltk {

TextStyle::TextStyle() :
    halign(TextHAlign::LEFT),
    valign(TextVAlign::CENTER),
    font_size(12) {}

void drawText(
    const std::string& text,
    const TextStyle& text_style,
    double x,
    double y,
    Layer* layer) {
  auto ctx = layer->ctx;
  auto dpi = layer->dpi;

  auto hb_buf = hb_buffer_create();
  hb_buffer_set_unicode_funcs(hb_buf, hb_icu_get_unicode_funcs());
  hb_buffer_set_direction(hb_buf, HB_DIRECTION_LTR);
  hb_buffer_set_script(hb_buf, HB_SCRIPT_LATIN);

  FT_Library ft_library;
  assert(!FT_Init_FreeType(&ft_library));

  FT_Face ft_face;
  assert(!FT_New_Face(ft_library, "/usr/share/fonts/google-roboto/Roboto-Regular.ttf", 0, &ft_face));
  assert(!FT_Set_Char_Size(ft_face, 0, text_style.font_size * 64, dpi, dpi));

  auto hb_font = hb_ft_font_create(ft_face, NULL);
  auto cairo_face = cairo_ft_font_face_create_for_ft_face(ft_face, 0);

  hb_buffer_add_utf8(hb_buf, text.data(), text.size(), 0, text.size());
  hb_shape(hb_font, hb_buf, NULL, 0);

  unsigned int glyph_count;
  auto hb_glyph_info = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
  auto hb_glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);
  auto cairo_glyphs = new cairo_glyph_t[glyph_count];

  double g_x = x;
  for (int i = 0; i < glyph_count; ++i) {
    cairo_glyphs[i].index = hb_glyph_info[i].codepoint;
    cairo_glyphs[i].x = g_x + (hb_glyph_pos[i].x_offset / 64.0);
    cairo_glyphs[i].y = y;
    g_x += (hb_glyph_pos[i].x_advance / 64.0);
  }

  cairo_set_source_rgba(ctx, 0.5, 0.5, 0.5, 1.0);
  cairo_set_font_face(ctx, cairo_face);
  cairo_set_font_size(ctx, text_style.font_size);
  cairo_show_glyphs(ctx, cairo_glyphs, glyph_count);

  delete cairo_glyphs;
  hb_buffer_destroy(hb_buf);
}

} // namespace signaltk


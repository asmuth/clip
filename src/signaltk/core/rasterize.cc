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
#include <agg2/agg_basics.h>
#include <agg2/agg_rasterizer_scanline_aa.h>
#include <agg2/agg_path_storage.h>
#include <agg2/agg_scanline_u.h>
#include <agg2/agg_conv_curve.h>

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

Status Rasterizer::rasterizePath(
    const PathData* point_data,
    size_t point_count) {
  if (point_count < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  agg::rasterizer_scanline_aa<> rasterizer;
  rasterizer.reset();
  //rasterizer.filling_rule(agg::fill_even_odd);
  //rasterizer.gamma(agg::gamma_power(m_gamma.value() * 2.0));

  agg::path_storage agg_path;
  for (size_t i = 0; i < point_count; ++i) {
    const auto& d = point_data[i];

    switch (d.command) {
      case PathCommand::MOVE_TO:
        agg_path.move_to(d.coefficients[0], d.coefficients[1]);
        break;
      case PathCommand::LINE_TO:
        agg_path.line_to(d.coefficients[0], d.coefficients[1]);
        break;
      case PathCommand::QUADRATIC_CURVE_TO:
        agg_path.curve3(
            d.coefficients[0],
            d.coefficients[1],
            d.coefficients[2],
            d.coefficients[3]);
        break;
      case PathCommand::CUBIC_CURVE_TO:
        agg_path.curve4(
            d.coefficients[0],
            d.coefficients[1],
            d.coefficients[2],
            d.coefficients[3],
            d.coefficients[4],
            d.coefficients[5]);
        break;
      case PathCommand::CLOSE:
        agg_path.close_polygon();
        break;
    }
  }

  agg::conv_curve<agg::path_storage> agg_path_polyline(agg_path);
  rasterizer.add_path(agg_path_polyline);

  agg::scanline_u8 scanline;
  scanline.reset(rasterizer.min_x(), rasterizer.max_x());
  while (rasterizer.sweep_scanline(scanline)) {
    for (size_t j = 0; j < scanline.num_spans(); ++j) {
      auto span = scanline.begin() + j;

      for (size_t i = 0; i < std::abs(span->len); ++i) {
        double v = span->covers[i] / 255.0f;
        uint32_t ox = span->x + i;
        uint32_t oy = scanline.y();

        if (ox >= pixmap->getWidth() || oy >= pixmap->getHeight()) {
          continue;
        }

        // FIXME alpha blend instead
        pixmap->setPixel(ox, oy, Colour::fromRGBA(v, v, v, 1));
      }
    }
  }

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


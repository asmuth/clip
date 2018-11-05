/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <graphics/rasterize.h>
#include <graphics/image.h>

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

  cr_surface = cairo_image_surface_create(
      CAIRO_FORMAT_A8,
      pixmap->getWidth(),
      pixmap->getHeight());

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
/* FIXME: this is stupid.... */
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

  auto cr_bitmap = cairo_image_surface_get_data(cr_surface);
  auto width = pixmap->getWidth();
  auto height = pixmap->getHeight();
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      auto v = 1.0 - (cr_bitmap[y * width + x] / 255.0);

      if (v == 1.0f) {
        continue;
      }

      // FIXME alpha blend instead
      pixmap->setPixel(x, y, Colour::fromRGBA(v, v, v, 1));
    }
  }
}

/* rasterize path using Maxim Shemanarev's libagg */
/*
Status Rasterizer::rasterizePath(
    const PathData* point_data,
    size_t point_count) {
  if (point_count < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  agg::rasterizer_scanline_aa<> rasterizer;
  rasterizer.reset();
  rasterizer.filling_rule(agg::fill_even_odd);
  //rasterizer.gamma(agg::gamma_power(m_gamma.value() * 2.0));

  agg::path_storage agg_path;
  for (size_t i = 0; i < point_count; ++i) {
    const auto& d = point_data[i];

    switch (d.command) {
      case PathCommand::MOVE_TO:
        printf("...move %f, %f\n", d.coefficients[0], d.coefficients[1]);
        rasterizer.move_to(d.coefficients[0], d.coefficients[1]);
        break;
      case PathCommand::LINE_TO:
        printf("...lien %f, %f\n", d.coefficients[0], d.coefficients[1]);
        rasterizer.line_to(d.coefficients[0], d.coefficients[1]);
        break;
      case PathCommand::QUADRATIC_CURVE_TO:
        //agg_path.curve3(
        //    d.coefficients[0],
        //    d.coefficients[1],
        //    d.coefficients[2],
        //    d.coefficients[3]);
        break;
      case PathCommand::CUBIC_CURVE_TO:
        //agg_path.curve4(
        //    d.coefficients[0],
        //    d.coefficients[1],
        //    d.coefficients[2],
        //    d.coefficients[3],
        //    d.coefficients[4],
        //    d.coefficients[5]);
        break;
      case PathCommand::CLOSE:
        agg_path.close_polygon();
        break;
    }
  }

  printf("... %u, %u\n", rasterizer.min_x(), rasterizer.max_x());

  //agg::conv_curve<agg::path_storage> agg_path_polyline(agg_path);
  //rasterizer.add_path(agg_path_polyline);

  //agg::scanline_u8 scanline;
  //scanline.reset(rasterizer.min_x(), 200);
  //while (rasterizer.sweep_scanline(scanline)) {
  //  for (size_t j = 0; j < scanline.num_spans(); ++j) {
  //    //auto span = scanline.begin() + j;

  //    //for (size_t i = 0; i < std::abs(span->len); ++i) {
  //    //  double v = span->covers[i] / 255.0f;
  //    //  uint32_t ox = span->x + i;
  //    //  uint32_t oy = scanline.y();

  //    //  if (ox >= pixmap->getWidth() || oy >= pixmap->getHeight()) {
  //    //    continue;
  //    //  }

  //    //  printf("setpixel %u %u\n", ox, oy);

  //    //  // FIXME alpha blend instead
  //    //  pixmap->setPixel(ox, oy, Colour::fromRGBA(v, v, v, 1));
  //    //}
  //  }
  //}

  return OK;
}
*/

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


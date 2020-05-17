/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "graphics/rasterize.h"
#include "graphics/image.h"
#include "graphics/text_layout.h"
#include "graphics/shape_hatch.h"

#include <iostream>

namespace clip {

Rasterizer::Rasterizer(
    uint32_t width_,
    uint32_t height_,
    double dpi_) :
    width(width_),
    height(height_),
    dpi(dpi_) {
  cr_surface = cairo_image_surface_create(
      CAIRO_FORMAT_ARGB32,
      width,
      height);

  cr_ctx = cairo_create(cr_surface);
}

Rasterizer::~Rasterizer() {
  cairo_destroy(cr_ctx);
  cairo_surface_destroy(cr_surface);
}

void cairo_set_path(
    cairo_t* cr_ctx,
    uint32_t height,
    const Path& path) {
  cairo_new_path(cr_ctx);

  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        cairo_move_to(cr_ctx, cmd[0], height - cmd[1]);
        break;
      case PathCommand::LINE_TO:
        cairo_line_to(cr_ctx, cmd[0], height - cmd[1]);
        break;
      case PathCommand::CLOSE:
        cairo_close_path(cr_ctx);
        break;
    }
  }
}

Status Rasterizer::drawShape(
    const Path& path,
    const StrokeStyle& stroke_style,
    const FillStyle& fill_style) {
  if (path.size() < 2) {
    return ERROR;
  }

  auto fill_path = path;
  if (fill_style.hatch) {
    fill_path = shape_hatch(
        path_to_polygon_simple(path),
        fill_style.hatch_angle_deg,
        fill_style.hatch_offset,
        fill_style.hatch_stride,
        fill_style.hatch_width);
  }

  if (fill_style.color) {
    cairo_set_source_rgba(
       cr_ctx,
       fill_style.color->red(),
       fill_style.color->green(),
       fill_style.color->blue(),
       fill_style.color->alpha());

    cairo_set_path(cr_ctx, height, fill_path);
    cairo_fill(cr_ctx);
  }

  if (stroke_style.line_width.value) {
    switch (stroke_style.dash_type) {
      case StrokeStyle::SOLID:
        cairo_set_dash(cr_ctx, nullptr, 0, 0);
        break;
      case StrokeStyle::DASH: {
        std::vector<double> dash_pattern;
        for (const auto& v : stroke_style.dash_pattern) {
          dash_pattern.push_back(v);
        }

        cairo_set_dash(
            cr_ctx,
            dash_pattern.data(),
            dash_pattern.size(),
            stroke_style.dash_offset);
        break;
      }
    }

    cairo_set_source_rgba(
       cr_ctx,
       stroke_style.color.red(),
       stroke_style.color.green(),
       stroke_style.color.blue(),
       stroke_style.color.alpha());

    cairo_set_line_width(cr_ctx, stroke_style.line_width.value);
    cairo_set_path(cr_ctx, height, path);
    cairo_stroke(cr_ctx);
  }

  return OK;
}

Status Rasterizer::drawText(
    const std::vector<text::GlyphPlacementGroup>& glyphs,
    const TextStyle& style,
    const Option<mat3>& transform) {
  if (transform) {
    auto transform_m = mul(
        mul(scale2({1, -1}), translate2({0, -double(height)})),
        mul(*transform, mul(translate2({0, double(height)}), scale2({1, -1}))));

    cairo_matrix_t m;
    m.xx = transform_m.a;
    m.xy = transform_m.b;
    m.yx = transform_m.d;
    m.yy = transform_m.e;
    m.x0 = transform_m.c;
    m.y0 = transform_m.f;
    cairo_set_matrix(cr_ctx, &m);
  } else {
    cairo_identity_matrix(cr_ctx);
  }

  for (const auto& gg : glyphs) {
    auto ft_font = static_cast<FT_Face>(font_get_freetype(gg.font));
    auto font_size_pt = unit_to_pt(style.font_size, dpi);
    auto font_size_ft = font_size_pt.value * (72.0 / dpi) * 64;
    if (FT_Set_Char_Size(ft_font, 0, font_size_ft, dpi, dpi)) {
      FT_Done_Face(ft_font);
      return ERROR;
    }

    cairo_set_source_rgba(
       cr_ctx,
       style.color.red(),
       style.color.green(),
       style.color.blue(),
       style.color.alpha());

    auto cairo_face = cairo_ft_font_face_create_for_ft_face(ft_font, 0);
    cairo_set_font_face(cr_ctx, cairo_face);
    cairo_set_font_size(cr_ctx, font_size_pt.value);

    auto glyph_count = gg.glyphs.size();
    auto cairo_glyphs = cairo_glyph_allocate(glyph_count);
    for (int i = 0; i < glyph_count; ++i) {
      const auto& g = gg.glyphs[i];

      cairo_glyphs[i].index = g.codepoint;
      cairo_glyphs[i].x = g.x;
      cairo_glyphs[i].y = height - g.y;
    }

    cairo_show_glyphs(cr_ctx, cairo_glyphs, glyph_count);
    cairo_glyph_free(cairo_glyphs);
    cairo_font_face_destroy(cairo_face);
  }

  return OK;
}

void Rasterizer::clear(const Color& c) {
  cairo_set_source_rgba(
      cr_ctx,
      c.red(),
      c.green(),
      c.blue(),
      c.alpha());

  cairo_paint(cr_ctx);
}

const unsigned char* Rasterizer::data() const {
  return cairo_image_surface_get_data(cr_surface);
}

size_t Rasterizer::size() const {
  return width * height * 4;
}

cairo_status_t cr_copy(
    void* closure,
    const unsigned char* data,
    unsigned int length) {
  *static_cast<std::string*>(closure) += std::string((const char*) data, length);
  return CAIRO_STATUS_SUCCESS;
}

std::string Rasterizer::to_png() const {
  std::string buf;
  cairo_surface_write_to_png_stream(cr_surface, cr_copy, &buf);
  return buf;
}

Status Rasterizer::writeToFile(const std::string& path) {
  if (StringUtil::endsWith(path, ".png")) {
    auto rc = cairo_surface_write_to_png(cr_surface, path.c_str());
    if (rc == CAIRO_STATUS_SUCCESS) {
      return OK;
    } else {
      return ERROR;
    }
  }

  return ERROR;
}

} // namespace clip


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
#include <graphics/text_layout.h>

namespace plotfx {

Rasterizer::Rasterizer(
    uint32_t width,
    uint32_t height,
    MeasureTable measures_,
    std::shared_ptr<text::TextShaper> text_shaper_) :
    measures(measures_),
    text_shaper(text_shaper_),
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

Status Rasterizer::fillPath(const layer_ops::BrushFillOp& op) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;
  if (path.size() < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  cairo_set_source_rgba(
     cr_ctx,
     style.colour.red(),
     style.colour.green(),
     style.colour.blue(),
     style.colour.alpha());

  cairo_rectangle(cr_ctx, clip.x, clip.y, clip.w, clip.h);
  cairo_clip(cr_ctx);
  cairo_new_path(cr_ctx);

  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        cairo_move_to(cr_ctx, cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        cairo_line_to(cr_ctx, cmd[0], cmd[1]);
        break;
      case PathCommand::ARC_TO:
        cairo_arc(cr_ctx, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
        break;
    }
  }

  cairo_fill(cr_ctx);

  return OK;
}

Status Rasterizer::strokePath(const layer_ops::BrushStrokeOp& op) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;

  if (path.size() < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  cairo_set_source_rgba(
     cr_ctx,
     style.colour.red(),
     style.colour.green(),
     style.colour.blue(),
     style.colour.alpha());

  cairo_set_line_width(cr_ctx, to_px(measures, style.line_width));

  cairo_rectangle(cr_ctx, clip.x, clip.y, clip.w, clip.h);
  cairo_clip(cr_ctx);
  cairo_new_path(cr_ctx);

  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        cairo_move_to(cr_ctx, cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        cairo_line_to(cr_ctx, cmd[0], cmd[1]);
        break;
      case PathCommand::ARC_TO:
        cairo_arc(cr_ctx, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
        break;
    }
  }

  cairo_stroke(cr_ctx);

  return OK;
}

Status Rasterizer::drawText(const layer_ops::TextSpanOp& op) {
  std::vector<GlyphPlacement> glyphs;
  auto rc = text::layoutText(
      op.text,
      op.x,
      op.y,
      op.style.font,
      op.style.font_size,
      measures.dpi,
      op.style.direction,
      op.style.halign,
      op.style.valign,
      text_shaper.get(),
      [&glyphs] (const GlyphPlacement& g) { glyphs.emplace_back(g); });

  if (rc != OK) {
    return rc;
  }

  return drawTextGlyphs(
      glyphs.data(),
      glyphs.size(),
      op.style);
}

Status Rasterizer::drawTextGlyphs(
    const GlyphPlacement* glyphs,
    size_t glyph_count,
    const TextStyle& style) {
  if (!ft_ready) {
    return ERROR;
  }

  // FIXME cache
  FT_Face ft_font;
  if (FT_New_Face(ft, style.font.font_file.c_str(), 0, &ft_font)) {
    return ERROR;
  }

  auto dpi = measures.dpi;
  if (FT_Set_Char_Size(ft_font, 0, style.font_size * 64, dpi, dpi)) {
    FT_Done_Face(ft_font);
    return ERROR;
  }

  cairo_set_source_rgba(
     cr_ctx,
     style.colour.red(),
     style.colour.green(),
     style.colour.blue(),
     style.colour.alpha());

  auto cairo_face = cairo_ft_font_face_create_for_ft_face(ft_font, 0);
  cairo_set_font_face(cr_ctx, cairo_face);
  cairo_set_font_size(cr_ctx, (style.font_size / 72.0) * dpi);

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

void Rasterizer::clear(const Colour& c) {
  cairo_set_source_rgba(
      cr_ctx,
      c.red(),
      c.green(),
      c.blue(),
      c.alpha());

  cairo_paint(cr_ctx);
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
      return ERROR_IO;
    }
  }

  return ERROR_INVALID_ARGUMENT;
}

} // namespace plotfx


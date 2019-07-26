/**
 * This file is part of the "fviz" project
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
#include <graphics/text_shaper.h>
#include <iostream>

#include <boost/locale.hpp>

using std::placeholders::_1;

namespace fviz {
namespace text {

Status text_shape(
    const std::string& text,
    FontRef font,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs) {
  std::unique_ptr<hb_buffer_t, std::function<void (hb_buffer_t*)>> hb_buf_ref(hb_buffer_create(), bind(&hb_buffer_destroy, _1));

  auto hb_buf = hb_buf_ref.get();
  auto ft_font = static_cast<FT_Face>(font_get_freetype(font));

  auto font_size_ft = font_size * (72.0 / dpi) * 64;
  if (FT_Set_Char_Size(ft_font, 0, font_size_ft, dpi, dpi)) {
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
    g.font = font;
    g.codepoint = glyph_infos[i].codepoint;
    g.advance_x = glyph_positions[i].x_advance / 64.0;
    g.advance_y = glyph_positions[i].y_advance / 64.0;
    g.metrics_ascender = ft_font->size->metrics.ascender / 64.0; // FIXME this is constant for all glyphs
    g.metrics_descender = ft_font->size->metrics.descender / 64.0; // FIXME this is constant for all glyphs
    glyphs->emplace_back(g);
  }

  hb_font_destroy(hb_font);

  return OK;
}

Status text_shape_with_font_fallback(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs) {

  boost::locale::generator locale_gen;
  boost::locale::boundary::ssegment_index grapheme_iter(
        boost::locale::boundary::character,
        text.begin(),
        text.end(),
        locale_gen("en_US.UTF-8"));

  // TODO: try to create the longest runs possible with the same font by
  // backgracking
  for (const auto& grapheme : grapheme_iter) {
    std::vector<GlyphInfo> grapheme_glyphs;

    for (const auto& font : font_info.fonts) {
      grapheme_glyphs.clear();

      auto rc = text_shape(grapheme, font, font_size, dpi, &grapheme_glyphs);
      if (rc != OK) {
        return rc;
      }

      bool font_ok = true;
      for (const auto& g : grapheme_glyphs) {
        if (g.codepoint == 0) {
          font_ok = false;
          break;
        }
      }

      if (font_ok) {
        break;
      }
    }

    glyphs->insert(glyphs->end(), grapheme_glyphs.begin(), grapheme_glyphs.end());
  }

  return OK;
}

} // namespace text
} // namespace fviz


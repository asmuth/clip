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

#include <ft2build.h>
#include FT_FREETYPE_H
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

using std::placeholders::_1;

namespace fviz {
namespace text {

Status text_shape_run(
    const std::string& text,
    TextDirection text_direction,
    const std::string& language,
    const std::string& script,
    FontRef font,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs) {
  /* get freetype font */
  auto ft_font = static_cast<FT_Face>(font_get_freetype(font));
  auto font_size_ft = font_size * (72.0 / dpi) * 64;
  if (FT_Set_Char_Size(ft_font, 0, font_size_ft, dpi, dpi)) {
    return ERROR;
  }

  std::unique_ptr<
      hb_font_t,
      std::function<void (hb_font_t*)>>
      hb_font(
          hb_ft_font_create_referenced(ft_font),
          bind(&hb_font_destroy, _1));

  /* prepare buffer */
  std::unique_ptr<
      hb_buffer_t,
      std::function<void (hb_buffer_t*)>>
      hb_buf(
          hb_buffer_create(),
          bind(&hb_buffer_destroy, _1));

  hb_buffer_reset(hb_buf.get());

  if (!language.empty()) {
    hb_buffer_set_language(
        hb_buf.get(),
        hb_language_from_string(language.c_str(), language.length()));
  }

  if (!script.empty()) {
    hb_buffer_set_script(
        hb_buf.get(),
        hb_script_from_string(script.c_str(), script.length()));
  }

  switch (text_direction) {
    case TextDirection::LTR:
      hb_buffer_set_direction(hb_buf.get(), HB_DIRECTION_LTR);
      break;
    case TextDirection::RTL:
      hb_buffer_set_direction(hb_buf.get(), HB_DIRECTION_RTL);
      break;
  }

  hb_buffer_add_utf8(hb_buf.get(), text.data(), text.size(), 0, text.size());

  /* shape */
  hb_shape(hb_font.get(), hb_buf.get(), NULL, 0);

  /* output glyph info */
  uint32_t glyph_count;
  auto glyph_infos = hb_buffer_get_glyph_infos(hb_buf.get(), &glyph_count);
  auto glyph_positions = hb_buffer_get_glyph_positions(hb_buf.get(), &glyph_count);
  for (size_t i = 0; i < glyph_count; ++i) {
    GlyphInfo g;
    g.font = font;
    g.codepoint = glyph_infos[i].codepoint;
    g.advance_x = glyph_positions[i].x_advance / 64.0;
    g.advance_y = glyph_positions[i].y_advance / 64.0;
    g.metrics_ascender = ft_font->size->metrics.ascender / 64.0;
    g.metrics_descender = ft_font->size->metrics.descender / 64.0;
    glyphs->emplace_back(g);
  }

  return OK;
}

Status text_shape_run_with_font_fallback(
    const std::string& text,
    TextDirection text_direction,
    const std::string& text_language,
    const std::string& text_script,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs) {

  std::vector<GlyphInfo> font_glyphs;
  for (const auto& font : font_info.fonts) {
    font_glyphs.clear();

    auto rc = text_shape_run(
        text,
        text_direction,
        text_language,
        text_script,
        font,
        font_size,
        dpi,
        &font_glyphs);

    if (rc != OK) {
      return rc;
    }

    bool font_ok = true;
    for (const auto& g : font_glyphs) {
      if (g.codepoint == 0) {
        font_ok = false;
        break;
      }
    }

    if (font_ok) {
      break;
    }
  }

  glyphs->insert(glyphs->end(), font_glyphs.begin(), font_glyphs.end());

  return OK;
}

} // namespace text
} // namespace fviz


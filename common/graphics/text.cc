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
#include <graphics/text.h>
#include <graphics/text_shaper.h>
#include "graphics/text_layout.h"
#include <graphics/layer.h>

namespace plotfx {

TextStyle::TextStyle() :
    direction(TextDirection::LTR),
    halign(TextHAlign::LEFT),
    valign(TextVAlign::BASELINE),
    font_size(12) {}

Status drawText(
    const std::string& text,
    double x,
    double y,
    const TextStyle& style,
    Layer* layer) {
  FontInfo font_info {
    .font_file = "/usr/share/fonts/google-roboto/Roboto-Medium.ttf",
    .font_size = style.font_size
  };

  std::vector<GlyphPlacement> glyphs;
  auto rc = text::layoutText(
      text,
      x,
      y,
      font_info,
      style.direction,
      style.halign,
      style.valign,
      &layer->text_shaper,
      [&glyphs] (const GlyphPlacement& g) { glyphs.emplace_back(g); });

  if (rc != OK) {
    return rc;
  }

  return drawTextGlyphs(font_info, glyphs.data(), glyphs.size(), style, layer);
}

Status drawTextGlyphs(
    const FontInfo& font_info,
    const GlyphPlacement* glyphs,
    size_t glyph_count,
    const TextStyle& style,
    Layer* layer) {
  return layer->rasterizer.drawTextGlyphs(font_info, glyphs, glyph_count, style);
}

} // namespace plotfx


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
#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

#include "text.h"

namespace fviz {
namespace text {

struct GlyphInfo {
  FontRef font;
  uint32_t codepoint;
  double advance_y;
  double advance_x;
  double metrics_ascender;
  double metrics_descender;
};

Status text_shape(
    const std::string& text,
    TextDirection text_direction,
    FontRef font,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

Status text_shape_with_font_fallback(
    const std::string& text,
    TextDirection text_direction,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

} // namespace text
} // namespace fviz


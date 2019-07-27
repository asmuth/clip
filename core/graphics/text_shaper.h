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

using GlyphRun = std::vector<GlyphInfo>;

/**
 * Shape a "run" of text with a given font
 */
Status text_shape_run(
    const std::string& text,
    TextDirection text_direction,
    FontRef font,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

/**
 * Shape a "run" of text with font fallback. 
 */
Status text_shape_run_with_font_fallback(
    const std::string& text,
    TextDirection text_direction,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

/**
 * "Shape" a non-breakable span of text. Input is a UTF8 string in logical order
 * and the intended text rendering direction. Output is a "glyph run", i.e. a
 * list of glyphs in the order in which they should be displayed. Note that this
 * method performs bidi reordering to 'visual' order, so the output of this
 * method is not suitable for line breaking. If line breaking is desired, it
 * must be handled by a mechanism higher up in the stack that only calls `text_shape`
 * for non-breakable spans of text (i.e. words).
 */
Status text_shape(
    const std::string& text,
    TextDirection text_direction,
    FontRef font,
    double font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

} // namespace text
} // namespace fviz


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
#pragma once
#include <string>
#include <unordered_map>
#include <functional>

#include "text.h"

namespace clip {
namespace text {

struct GlyphInfo {
  FontRef font;
  uint32_t codepoint;
  double advance_y;
  double advance_x;
  double metrics_ascender;
  double metrics_descender;
};

/**
 * Shape a "run" of text with a given font and a given text direction. The
 * text must be provided as a UTF-8 string in logical character order.
 */
Status text_shape_run(
    const std::string& text,
    TextDirection text_direction,
    const std::string& text_language,
    const std::string& text_script,
    FontRef font,
    NumberPT font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

/**
 * Shape a "run" of UTF-8 text in logical character order with font fallback
 */
Status text_shape_run_with_font_fallback(
    const std::string& text,
    TextDirection text_direction,
    const std::string& text_language,
    const std::string& text_script,
    const FontInfo& font_info,
    NumberPT font_size,
    double dpi,
    std::vector<GlyphInfo>* glyphs);

} // namespace text
} // namespace clip


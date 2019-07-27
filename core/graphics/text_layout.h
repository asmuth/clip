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
#include <graphics/text.h>
#include <graphics/layout.h>

namespace fviz {
namespace text {
class TextShaper;

struct GlyphPlacement {
  uint32_t codepoint;
  double x;
  double y;
};

struct GlyphSpan {
  FontRef font;
  std::vector<GlyphPlacement> glyphs;
};

/**
 * Measure the size of a horizontal span of text where (0, 0) is the baseline of
 * the first glyph
 */
Status text_measure_span(
    const std::string& text,
    const FontInfo& font_info,
    double font_size,
    double dpi,
    Rectangle* rect);

/**
 * Layout a horizontal span of text
 */
Status text_layout_hspan(
    const std::string& text,
    TextDirection direction,
    const FontInfo& font,
    double font_size,
    double dpi,
    std::vector<GlyphSpan>* spans,
    Rectangle* bbox);

} // namespace text
} // namespace fviz


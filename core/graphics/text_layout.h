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

/**
 * A simplistic text layout engine with basic support for internationalization.
 *
 * Note that the text layout code is slow and performs duplicate work in some
 * places. Whenever there is a tradeoff between simplicity and performance the
 * most simple approach is used. Accordingly, the layout engine is only suitable
 * for offline rendering.
 */
namespace fviz::text {

/**
 * A text span represents a discrete, non-breakable piece of text. Text spans are
 * the smallest unit of text as far as the layout code is concerned. All text
 * in a text span must have the same font, font size and script.
 *
 * The text data of this span is a UTF-8 encoded string in logical character
 * order.
 */
struct TextSpan {
  std::string text;
  TextDirection text_direction;
  std::string language;
  std::string script;
  FontInfo font;
  double font_size;
};


/**
 * The output of the text layout process is a list of glyph placements. The
 * coordinates are absolute screen positions
 */
struct GlyphPlacement {
  FontRef font;
  uint32_t codepoint;
  double x;
  double y;
};


/**
 * A glyph placement group contains a list of placed glyphs where each glyph
 * has the same font
 */
struct GlyphPlacementGroup {
  FontRef font;
  std::vector<GlyphPlacement> glyphs;
};


/**
 * Layout a line of text. The text must already be itemized into spans so that
 * all text in any span has the same font, font size and script.
 *
 * Note that at this point only LTR and RTL text as well as bidirectional LTR/RTL
 * text is supported. Vertical (TTB) line layout is not yet implemented.
 */
Status text_layout_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    double dpi,
    std::vector<GlyphPlacementGroup>* glyphs,
    Rectangle* bbox);


/**
 * Layout a single line of text. The input text must be provided in UTF-8
 * encoding and in logical character order. The `text_direction_base` argument
 * controls the base writing direction of the line
 *
 * Note that at this point only LTR and RTL text as well as bidirectional LTR/RTL
 * text is supported. Vertical (TTB) line layout is not yet implemented.
 */
Status text_layout_line(
    const std::string& text,
    TextDirection text_direction_base,
    const FontInfo& font,
    double font_size,
    double dpi,
    std::vector<GlyphPlacementGroup>* glyphs,
    Rectangle* bbox);


} // namespace fviz::text


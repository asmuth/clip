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

namespace fviz::text {


/**
 * A text span represents a discrete, non-breakable piece of text. Text spans are
 * the smallest unit of text for layout. Text spans are the input to the text
 * layout process.
 *
 * If any kind of line wrapping is desired, the input text must be split into
 * text spans so that line breaking can be performed on a per-span basis.
 *
 * One important caveat of this is that text shaping will also be performed on a
 * per-span basis, so the spans should be as large as possible for the best text
 * shaping results. Note that font fallback selection is also performed per-span,
 * so ideally any span should be renderable with a single font from the font
 * stack.
 *
 * In latin scripts, text spans should probably correspond to word boundaries.
 * In other scripts, text spans should correspond to whatever unit of text is
 * small enough to allow for text breaking on span-level but large enough so that
 * per-span shaping of text is sufficient.
 *
 * It is allowed to break the line after or before each text span, i.e. spans
 * must be given so that it is legal to place each text span at the beginning of
 * a new line. However it is *not* allowed to break a span itself into smaller
 * pieces; all text in the span must be put onto the same line.
 *
 * This interface should enable us to have a high degree of decoupling between
 * the text shaping and layout parts. However, one tradeoff is that it does not
 * allow users to implement line breaking at  character boundaries (i.e breaking
 * and hyphenization of words).
 *
 * The text data of this span as a UTF-8 encoded string in logical character
 * order
 */
struct TextSpan {
  std::string text;
  std::string language;
  std::string script;
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
 * Layout a line of text. The text should already be itemized into spans.
 *
 * Note that at this point only LTR and RTL text as well as bidirectional LTR/RTL
 * text is supported. Vertical (TTB) line layout is not yet implemented.
 */
Status text_layout_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    const FontInfo& font,
    double font_size,
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


/**
 * Measure the size of a single line of text.
 */
Status text_measure_line(
    const std::string& text,
    TextDirection text_direction_base,
    const FontInfo& font,
    double font_size,
    double dpi,
    Rectangle* bbox);


} // namespace fviz::text


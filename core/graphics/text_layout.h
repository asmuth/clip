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
 * the smallest unit of text for layout.
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
 * A line of text
 *
 * The `text_runs` member contains the line's text runs as a UTF-8 strings.
 * Note that the runs are given in logical order and the characters within
 * the runs are also stored in logical order.
 *
 * Non-bidirectional text spans should usually have exactly one text run while
 * bidirectional text should have N + 1 runs where N is the number of writing
 * direction boundaries in the text span.
 */
struct TextLine {
  std::vector<std::string> text_runs;
  std::vector<TextDirection> text_directions;
  std::vector<const TextSpan*> text_spans;
};


/**
 * The output of the text layouting process is a list of glyph placements. The
 * coordinates are absolute screen positions
 */
struct GlyphPlacement {
  FontRef font;
  uint32_t codepoint;
  double x;
  double y;
};

struct GlyphSpan {
  FontRef font;
  std::vector<GlyphPlacement> glyphs;
};


/**
 * Layout a horizontal line of text. The input text must be provided in UTF-8
 * encoding and in logical character order. The `text_direction_base` argument
 * controls the base writing direction of the line
 */
Status text_layout_hline(
    const TextLine& line,
    TextDirection base_direction,
    const FontInfo& font,
    double font_size,
    double dpi,
    std::vector<GlyphSpan>* spans,
    Rectangle* bbox);


/**
 * Layout a horizontal line of text. The input text must be provided in UTF-8
 * encoding and in logical character order. The `text_direction_base` argument
 * controls the base writing direction of the line
 */
Status text_layout_hline(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    const FontInfo& font,
    double font_size,
    double dpi,
    std::vector<GlyphSpan>* spans,
    Rectangle* bbox);


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


} // namespace fviz::text


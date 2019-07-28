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
 * the smallest unit of text as far as the layout code is concerned.
 *
 * The text data of a span is a UTF-8 encoded string in logical character
 * order. The span id may be used to map glyphs in the output to the span
 * that produced them.
 */
struct TextSpan {
  std::string text;
  TextDirection text_direction;
  std::string language;
  std::string script;
  FontInfo font;
  double font_size;
  uint32_t span_id;
};


/**
 * A line of text that has been prepared for final layout (placement).
 *
 * The `spans` member contains the line's "text spans". The `base_direction`
 * contains the inteded base writing direction for the line.
 *
 * While characters within a span are always stored in logical order, the layout
 * engine will place the spans in the order in which they are stored in the
 * `spans` list; the first element in the spans list will always be placed at the
 * "beginning" of the line in the corresponding base_direction, which is the left
 * edge for LTR text or the right edge for RTL text.
 *
 * For correct display of bi-directional text, there should be at least N + 1
 * spans where N is the number of writing direction boundaries in the line. The
 * spans within the line must be re-ordered into visual order before the layout
 * of the line is performed. The text within each span must be given in logical
 * order and each span must have a correct `text_direction` attribute. 
 */
struct TextLine {
  TextDirection base_direction;
  std::vector<TextSpan> spans;
  std::vector<size_t> visual_order;
};


/**
 * The output of the text layout process is a list of glyph placements. The
 * coordinates are returned in "text layout space" so that the begin of the
 * baseline of the (first) line of text is placed at (0, 0)
 */
struct GlyphPlacement {
  FontRef font;
  uint32_t codepoint;
  double x;
  double y;
  uint32_t span_id;
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
 * all text in each span has the same font, font size, script and writing
 * direction and so that spans are ordered "visually" (see above).
 *
 * Note that at this point only horizontal text lines are supported, vertical
 * writing direction is not yet implemented.
 */
Status text_layout_line(
    const TextLine& text_line,
    double dpi,
    std::vector<GlyphPlacementGroup>* glyphs,
    Rectangle* bbox);


} // namespace fviz::text


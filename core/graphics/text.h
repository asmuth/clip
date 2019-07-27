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
#include "fviz.h"
#include "path.h"
#include "color.h"
#include "measure.h"
#include "layout.h"
#include "font_lookup.h"

namespace fviz {
class Layer;

enum class TextDirection {
  LTR, RTL
};

/**
 * A text span represents a discrete, non-breakable piece of text. Text spans are
 * the smallest unit of text for layout.
 *
 * If any kind of line wrapping is desired, the input text must be split into
 * text spans so that line breaking can be performed on a per span basis. One
 * important caveat of this is that text shaping will also be performed on a
 * per-span basis, so the spans should be as large as possible for the best text
 * shaping results.
 *
 * In latin scripts, text spans should probably correspond to word boundaries.
 * In other scripts, text spans should correspond to whatever unit of text is
 * small enough to allow for text breaking on span-level but large enough so that
 * per-span shaping of text is sufficient.
 *
 * It is allowed to break the line after or before each text span, i.e. spans
 * must be given so that it is legal to place each text span at the beginning of
 * a new line. However it is *not* allowed to break a span itself into smaller
 * pieces; all text runs within the span must be put onto the same line.
 *
 * This interface should enable us to have a high degree of decoupling between
 * the text shaping and layout parts. However, one tradeoff is that it does not
 * allow users to implement some advanced features such as line breaking at
 * character boundaries (i.e breaking and hyphenization of words).
 *
 */
struct TextSpan {
  /**
   * The `text_runs` member contains the spans text runs as a UTF-8 strings.
   * Note that the runs are given in logical order and the characters within
   * the runs are also stored in logical order.
   *
   * Non-bidirectional text spans should usually have exactly one text run while
   * bidirectional text should have N + 1 runs where N is the number of writing
   * direction boundaries in the text span.
   */
  std::vector<std::string> text_runs;

  /**
   * Stores the bidi text direction for each run
   */
  std::vector<TextDirection> text_directions;
};

struct TextStyle {
  TextStyle();
  TextDirection direction;
  FontInfo font;
  Measure font_size;
  Color color;
};

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    const TextStyle& text_style,
    Layer* layer);

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    const TextStyle& text_style,
    Layer* layer);

Status drawTextLabel(
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    const TextStyle& text_style,
    Layer* layer);

} // namespace fviz


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
#include "fviz.h"
#include "graphics/text_backend.h"
#include "graphics/text_layout.h"
#include "graphics/text_shaper.h"

#include <numeric>

namespace fviz {
namespace text {

/**
 * A line of text that has been prepared for final layout (placement).
 *
 * The `spans` member contains the line's text runs as a UTF-8 strings.
 * Note that the runs are given in logical order and the characters within
 * the runs are also stored in logical order.
 *
 * Non-bidirectional text spans should usually have exactly one text run while
 * bidirectional text should have N + 1 runs where N is the number of writing
 * direction boundaries in the text span.
 *
 * The `base_direction` contains the inteded display writing direction for this
 * line.
 *
 * The visual order contains the order in which the runs should be displayed so
 * that the first element in the visual_order list is placed at the "beginning"
 * of the line in the corresponding base_direction.
 */
struct TextLine {
  TextDirection base_direction;
  std::vector<TextSpan> spans;
  std::vector<size_t> visual_order;
};

Status text_place_hrun(
    const TextSpan& span,
    double dpi,
    std::vector<GlyphPlacement>* glyphs,
    double* span_length,
    double* span_top,
    double* span_bottom) {
  std::vector<GlyphInfo> glyph_list;
  auto shaping_rc = text_shape_run_with_font_fallback(
      span.text,
      span.text_direction,
      span.language,
      span.script,
      span.font,
      span.font_size,
      dpi,
      &glyph_list);

  if (!shaping_rc) {
    return shaping_rc;
  }

  for (const auto& gi : glyph_list) {
    GlyphPlacement gp;
    gp.font = gi.font;
    gp.codepoint = gi.codepoint;
    gp.y = 0;
    gp.x = *span_length;
    gp.span_id = span.span_id;
    glyphs->emplace_back(gp);

    *span_length += gi.advance_x;
    *span_top = std::min(-gi.metrics_ascender, *span_top);
    *span_bottom = std::max(-gi.metrics_descender, *span_bottom);
  }

  return OK;
}

Status text_place_hline(
    const TextLine& text_line,
    double dpi,
    std::vector<GlyphPlacementGroup>* glyphs,
    Rectangle* bbox) {
  double line_top = 0.0;
  double line_bottom = 0.0;
  double line_length = 0;
  for (auto i : text_line.visual_order) {
    double span_length = 0.0;
    std::vector<GlyphPlacement> span_glyphs;
    auto rc = text_place_hrun(
        text_line.spans[i],
        dpi,
        &span_glyphs,
        &span_length,
        &line_top,
        &line_bottom);

    if (rc != OK) {
      return rc;
    }

    for (auto& gi : span_glyphs) {
      switch (text_line.base_direction) {
        case TextDirection::LTR:
          gi.x += line_length;
          break;
        case TextDirection::RTL:
          gi.x -= line_length;
          gi.x -= span_length;
          break;
      }

      // TODO merge glyph spans with the same font
      GlyphPlacementGroup gg;
      gg.font = gi.font;
      gg.glyphs.emplace_back(gi);

      if (glyphs) {
        glyphs->emplace_back(gg);
      }
    }

    line_length += span_length;
  }

  double line_left = 0.0;
  switch (text_line.base_direction) {
    case TextDirection::LTR:
      line_left = 0;
      break;
    case TextDirection::RTL:
      line_left = -line_length;
      break;
  }

  *bbox = Rectangle(
      line_left,
      line_top,
      line_length,
      line_bottom - line_top);

  return OK;
}

/**
 * Determine the visual order of text runs in a line according to the unicode
 * bidi algorithm
 *
 *   "From the highest level found in the text to the lowest odd level on each
 *    line, including intermediate levels not actually present in the text,
 *    reverse any contiguous sequence of characters that are at that level or
 *    higher."
 *
 * Source: https://unicode.org/reports/tr9/#Reordering_Resolved_Levels
 *
 */
std::vector<size_t> text_reorder_line(const std::vector<int>& bidi_levels) {
  std::vector<size_t> visual_order(bidi_levels.size(), 0);

  std::iota(
      visual_order.begin(),
      visual_order.end(),
      0);

  size_t level_max = *std::max_element(
      bidi_levels.begin(),
      bidi_levels.end());

  for (size_t level_cur = level_max; level_cur >= 1; --level_cur) {
    for (size_t range_begin = 0; range_begin < bidi_levels.size(); ) {
      // find the next contiguous range where level >= level_cur starting at
      // begin
      auto range_end = range_begin;
      for (;
          bidi_levels[range_end] >= level_cur &&
          range_end != bidi_levels.size();
          ++range_end);

      // if no such sequence starts at begin, try searching from the next index
      if (range_end == range_begin) {
        ++range_begin;
        continue;
      }

      // reverse runs in the range
      std::reverse(
          visual_order.begin() + range_begin,
          visual_order.begin() + range_end);

      // continue searching from the end of the swapped range
      range_begin = range_end;
    }
  }

  return visual_order;
}

Status text_layout_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    const TextDirection text_direction_base,
    double dpi,
    std::vector<GlyphPlacementGroup>* glyphs,
    Rectangle* bbox) {
  // prepare a new text line for layout
  TextLine text_line;
  text_line.base_direction = text_direction_base;

  // split spans using the unicode bidi algorithm and compute visual span order
  std::vector<int> bidi_levels;
  if (auto rc =
        text_analyze_bidi_line(
            text_begin,
            text_end,
            text_direction_base,
            &text_line.spans,
            &bidi_levels);
      !rc) {
    return ERROR;
  }

  text_line.visual_order = text_reorder_line(bidi_levels);

  // if the base direction is RTL, reverse the direction of all runs so that
  // the "first" element in the visual order array is the one at the begining
  // of the line in our base writing direction
  switch (text_direction_base) {
    case TextDirection::LTR:
      break;
    case TextDirection::RTL:
      std::reverse(
          text_line.visual_order.begin(),
          text_line.visual_order.end());
  }

  // place the text glyphs on the screen
  return text_place_hline(
      text_line,
      dpi,
      glyphs,
      bbox);
}

} // namespace text
} // namespace fviz


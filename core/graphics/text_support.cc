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
#include "text_backend.h"
#include "text_support.h"
#include "text_layout.h"

#include <numeric>

namespace fviz::text {

ReturnCode text_reorder_bidi_line(TextLine* line) {
  std::vector<TextSpan> spans;

  // Split spans of the line using the unicode bidi algorithm and extract the
  // per-span bidi embedding levels
  std::vector<int> bidi_levels;
  if (auto rc =
        text_analyze_bidi_line(
            &*line->spans.begin(),
            &*line->spans.end(),
            line->base_direction,
            &spans,
            &bidi_levels);
      !rc) {
    return rc;
  }

  // Determine the correct visual order of spans according to the unicode spec
  //
  //   "From the highest level found in the text to the lowest odd level on each
  //    line, including intermediate levels not actually present in the text,
  //    reverse any contiguous sequence of characters that are at that level or
  //    higher."
  //
  // Source: https://unicode.org/reports/tr9/#Reordering_Resolved_Levels
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

  // if the base direction is RTL, reverse the direction of all runs so that
  // the "first" element in the visual order array is the one at the begining
  // of the line in our base writing direction
  switch (line->base_direction) {
    case TextDirection::LTR:
      break;
    case TextDirection::RTL:
      std::reverse(visual_order.begin(), visual_order.end());
  }

  // store the re-ordered spans back into the line
  line->spans.clear();

  for (auto i : visual_order) {
    line->spans.push_back(spans[i]);
  }

  return OK;
}

} // namespace fviz::text


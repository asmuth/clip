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

#include <numeric>
#include <fribidi/fribidi.h>

namespace fviz::text {

ReturnCode text_analyze_bidi_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    std::vector<std::string>* runs,
    std::vector<int>* run_bidi_levels,
    std::vector<const TextSpan*>* run_span_map) {
  FriBidiParType fb_basedir;
  switch (text_direction_base) {
    case TextDirection::LTR:
      fb_basedir = FRIBIDI_PAR_LTR;
      break;
    case TextDirection::RTL:
      fb_basedir = FRIBIDI_PAR_RTL;
      break;
  }

  // convert to fribidi string
  size_t fb_str_maxlen = 0;
  for (auto text_iter = text_begin; text_iter != text_end; ++text_iter) {
    fb_str_maxlen += text_iter->text.size() * 4; // FIXME
  }

  std::vector<FriBidiChar> fb_str(fb_str_maxlen, 0);
  std::vector<const TextSpan*> fb_to_span_map;
  auto fb_str_len = 0;
  for (auto text_iter = text_begin; text_iter != text_end; ++text_iter) {
    auto len = fribidi_charset_to_unicode(
        FRIBIDI_CHAR_SET_UTF8,
        text_iter->text.data(),
        text_iter->text.size(),
        fb_str.data() + fb_str_len);

    for (size_t i = 0; i < len; ++i) {
      fb_to_span_map.push_back(text_iter);
    }

    fb_str_len += len;
  }

  // find character directionalities and embedding levels using fribidi
  std::vector<FriBidiCharType> fb_types(fb_str_len, 0);
  std::vector<FriBidiLevel> fb_levels(fb_str_len, 0);
  fribidi_get_bidi_types(fb_str.data(), fb_str_len, fb_types.data());

  auto fribidi_rc = fribidi_get_par_embedding_levels(
        fb_types.data(),
        fb_str_len,
        &fb_basedir,
        fb_levels.data());

  if (!fribidi_rc) {
    return errorf(ERROR, "error while performing bidi layout using fribidi");
  }

  // find the bidi runs in the output
  std::vector<size_t> run_bounds;
  int level_max = 0;
  for (size_t i = 0; i < fb_str_len; ++i) {
    level_max = std::max(level_max, int(fb_levels[i]));

    // split on level change
    if (i > 0 && fb_levels[i] != fb_levels[i - 1]) {
      run_bounds.emplace_back(i);
      continue;
    }

    // split on span id change
    if (i > 0 && fb_to_span_map[i] != fb_to_span_map[i - 1]) {
      run_bounds.emplace_back(i);
      continue;
    }
  }

  std::vector<int> levels;
  for (size_t i = 0; i < run_bounds.size() + 1; ++i) {
    auto run_begin = i == 0 ? 0 : run_bounds[i - 1];
    auto run_end = i == run_bounds.size() ? fb_str_len : run_bounds[i];
    auto run_len = run_end - run_begin;

    std::string run(run_len * 4 + 1, 0);
    run.resize(fribidi_unicode_to_charset(
        FRIBIDI_CHAR_SET_UTF8,
        fb_str.data() + run_begin,
        run_len,
        run.data()));

    runs->emplace_back(run);
    run_bidi_levels->emplace_back(int(fb_levels[run_begin]));
    run_span_map->emplace_back(fb_to_span_map[run_begin]);
  }

  return OK;
}

} // namespace fviz::text


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
#include "fviz_config.h"
#include "text_backend.h"
#include "text_backend_linux.h"

namespace fviz::text {

const constexpr std::string_view TEXT_BACKEND = FVIZ_TEXT_BACKEND;
const constexpr bool TEXT_ENABLE_BIDI = FVIZ_TEXT_ENABLE_BIDI;

ReturnCode text_analyze_bidi_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    std::vector<TextSpan>* runs,
    std::vector<int>* run_bidi_levels) {
  // fallback: no bidi analysis
  if constexpr (!TEXT_ENABLE_BIDI) {
    for (auto text_iter = text_begin; text_iter != text_end; ++text_iter) {
      auto run = *text_iter;
      runs->push_back(run);
      run_bidi_levels->push_back(0);
    }

    return OK;
  }

  // linux/freetype backend
  if constexpr (TEXT_BACKEND == "linux") {
    return backend_linux::text_analyze_bidi_line(
        text_begin,
        text_end,
        text_direction_base,
        runs,
        run_bidi_levels);
  }

  return errorf(ERROR, "no text backend found; BiDi analysis is not supported");
}

} // namespace fviz::text


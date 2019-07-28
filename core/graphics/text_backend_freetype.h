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
#include <string>
#include <unordered_map>
#include <functional>

#include "text.h"
#include "text_layout.h"

namespace fviz::text::backend_freetype {

/**
 * Analyze a UTF-8 input string in logical character order and produce a list
 * of text runs where each run has the same writing order as well as the bidi
 * embedding levels of each text run as defined by unicode.
 */
ReturnCode text_analyze_bidi_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    std::vector<TextSpan>* runs,
    std::vector<int>* run_bidi_levels);

} // namespace fviz::text::backend_freetype


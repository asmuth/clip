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

namespace fviz::text {

/**
 * Analyze a UTF-8 input string in logical character order and produce a text
 * line. The output text span will contain all text runs within the span in
 * logical order together with their corresponding directionality information.
 *
 * Note that this method returns a single text line, i.e. a non-breakable unit
 * of text. If line breaking is desired, identification of possible breakpoints
 * (spans) must be handled by a mechanism higher up in the stack.
 */
ReturnCode text_analyze_bidi_line(
    const TextSpan* text_begin,
    const TextSpan* text_end,
    TextDirection text_direction_base,
    TextLine* text_line);

} // namespace fviz::text


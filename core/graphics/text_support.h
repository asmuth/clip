/**
 * This file is part of the "clip" project
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
#include <graphics/text_layout.h>

namespace clip::text {

/**
 * Itemize the spans in a line by text directionality and reorder them into
 * "visual order" according to the unicode bidi algorithm. Note that only
 * the spans are re-ordered into visual order; text within spans stays in logical
 * order.
 */
ReturnCode text_reorder_bidi_line(TextLine* line);

} // namespace clip::text


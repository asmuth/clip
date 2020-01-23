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
#include <memory>
#include "return_code.h"
#include "path.h"

namespace clip {

struct FontStorage;
using FontRef = std::shared_ptr<FontStorage>;

struct FontInfo {
  std::vector<FontRef> fonts;
  std::string font_family_css;
  double font_weight_css;
};

ReturnCode font_load(
    const std::string& font_file,
    FontRef* font);

ReturnCode font_load_defaults(FontInfo* font_info);

ReturnCode font_load_best(
    const std::string& font_pattern,
    FontInfo* font_info);

ReturnCode font_get_glyph_path(
    FontRef font,
    double font_size,
    double dpi,
    uint32_t codepoint,
    Path* path);

void* font_get_freetype(FontRef font);

} // namespace clip


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
#include "text.h"

namespace fviz {

bool findFontSystem(
    const std::string& font_pattern,
    std::string* font_file);

enum DefaultFont {
  ROMAN_SANS_REGULAR,
  ROMAN_SANS_MEDIUM,
  ROMAN_SANS_BOLD,
  ROMAN_SERIF_REGULAR,
  ROMAN_SERIF_MEDIUM,
  ROMAN_SERIF_BOLD,
  ROMAN_MONOSPACE_REGULAR,
  ROMAN_MONOSPACE_MEDIUM,
  ROMAN_MONOSPACE_BOLD,
};

ReturnCode font_load(DefaultFont font_name, FontInfo* font_info);

ReturnCode font_configure(const Expr* expr, FontInfo* font_info);

} // namespace fviz


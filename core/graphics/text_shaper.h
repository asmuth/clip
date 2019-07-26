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
#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

#include "graphics/text_layout.h"
#include "text.h"

namespace fviz {
namespace text {

class TextShaper {
public:

  TextShaper();
  ~TextShaper();
  TextShaper(const TextShaper&) = delete;
  TextShaper& operator=(const TextShaper&) = delete;

  Status shapeText(
      const std::string& text,
      FontRef font,
      double font_size,
      double dpi,
      std::vector<GlyphInfo>* glyphs) const;

protected:
  double dpi;
  mutable hb_buffer_t* hb_buf;
};

} // namespace text
} // namespace fviz


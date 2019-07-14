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

#include <cairo.h>
#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

#include "text.h"
#include "brush.h"
#include "layout.h"
#include "layer.h"
#include "text_layout.h"

namespace fviz {
class Image;

class Rasterizer {
public:

  Rasterizer(uint32_t width, uint32_t height, double dpi, std::shared_ptr<text::TextShaper> text_shaper);
  ~Rasterizer();
  Rasterizer(const Rasterizer&) = delete;
  Rasterizer& operator=(const Rasterizer&) = delete;

  void clear(const Color& c);

  Status fillPath(const layer_ops::BrushFillOp& op);
  Status strokePath(const layer_ops::BrushStrokeOp& op);

  Status drawText(const layer_ops::TextSpanOp& op);
  Status drawTextGlyphs(
      const text::GlyphPlacement* glyphs,
      size_t glyph_count,
      const TextStyle& style);

  Status writeToFile(const std::string& path);

  std::string to_png() const;
  const unsigned char* data() const;
  size_t size() const;

  uint32_t width;
  uint32_t height;
  double dpi;
  std::shared_ptr<text::TextShaper> text_shaper;
  FT_Library ft;
  bool ft_ready;
  cairo_surface_t* cr_surface;
  cairo_t* cr_ctx;
};

using RasterizerRef = std::shared_ptr<Rasterizer>;

} // namespace fviz


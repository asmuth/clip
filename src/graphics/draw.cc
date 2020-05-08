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
#include "context.h"
#include "draw.h"

namespace clip {

void draw_polygon(
    Context* ctx,
    const Poly2& poly,
    StrokeStyle stroke_style,
    FillStyle fill_style) {
  convert_unit_typographic(
      layer_get_dpi(ctx),
      layer_get_font_size(ctx),
      &stroke_style.line_width);

  draw_cmd::Polygon elem;
  elem.poly = poly;
  elem.stroke_style = stroke_style;
  elem.fill_style = fill_style;
  layer_get(ctx)->drawlist.push_back(elem);
}

} // namespace clip


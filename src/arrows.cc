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
#include "arrows.h"
#include "graphics/brush.h"
#include <functional>

using namespace std::placeholders;
using std::bind;

namespace clip {

ReturnCode arrow_draw_default(
    Context* ctx,
    const Point& from,
    const Point& to,
    const Number& size,
    const Color& color) {
  auto direction = normalize(sub(to, from));
  auto ortho = vec2{direction.y, direction.x * -1};

  double head_width_factor = 2;
  double head_length_factor = 4;
  Path head_path;
  head_path.moveTo(add(to, mul(ortho, size.value * head_width_factor)));
  head_path.lineTo(sub(to, mul(ortho, size.value * head_width_factor)));
  head_path.lineTo(add(to, mul(direction, size.value * head_length_factor)));
  head_path.closePath();

  StrokeStyle line_style;
  line_style.color = color;
  line_style.line_width = size;

  FillStyle fill_style;
  fill_style.color = color;

  draw_line(ctx, from, to, line_style);
  draw_path(ctx, head_path, {}, fill_style);

  return OK;
}

Arrow arrow_create_default() {
  return std::bind(&arrow_draw_default, _1, _2, _3, _4, _5);
}

} // namespace clip


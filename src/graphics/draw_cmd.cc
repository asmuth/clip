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
#include "draw.h"
#include "context.h"
#include <graphics/text.h>
#include <graphics/text_shaper.h>
#include "graphics/text_layout.h"
#include "graphics/text_support.h"
#include "draw/text.h"

namespace clip {

void draw_shape(Context* ctx, DrawCommand shape) {
  auto layer = layer_get(ctx);

  if (shape.fill_style.color) {
    if (shape.fill_style.hatch) {
      shape.style.fill_hatch.push_back({
        .color = *shape.fill_style.color,
        .angle_deg = shape.fill_style.hatch_angle_deg,
        .offset = Number(shape.fill_style.hatch_offset),
        .stride = Number(shape.fill_style.hatch_stride),
        .width = Number(shape.fill_style.hatch_width),
      });
    } else {
      shape.style.fill_solid.emplace_back(*shape.fill_style.color);
    }

    shape.fill_style = {};
  }

  if (shape.stroke_style.line_width.value) {
    switch (shape.stroke_style.dash_type) {
      case StrokeStyle::SOLID:
        shape.style.stroke_solid.push_back({
          .color = shape.stroke_style.color,
          .width = shape.stroke_style.line_width
        });
        break;
      case StrokeStyle::DASH:
        std::vector<Number> pattern(shape.stroke_style.dash_pattern.size());
        std::transform(
            shape.stroke_style.dash_pattern.begin(),
            shape.stroke_style.dash_pattern.end(),
            pattern.begin(),
            [] (auto v) { return Number(v); });

        shape.style.stroke_dash.push_back({
          .color = shape.stroke_style.color,
          .width = shape.stroke_style.line_width,
          .pattern = pattern,
          .offset = Number(shape.stroke_style.dash_offset),
        });
        break;
    }

    shape.stroke_style = {};
  }

  layer_get(ctx)->drawlist.emplace_back(std::move(shape));
}

void draw_path(
    Context* ctx,
    const Path& path,
    StrokeStyle stroke_style,
    FillStyle fill_style) {
  DrawCommand shape;
  shape.path = path;
  shape.stroke_style = stroke_style;
  shape.fill_style = fill_style;
  draw_shape(ctx, shape);
}

void draw_polygon(
    Context* ctx,
    const Poly2& poly,
    StrokeStyle stroke_style,
    FillStyle fill_style) {
  DrawCommand shape;
  shape.path = path_from_poly2(poly);
  shape.stroke_style = stroke_style;
  shape.fill_style = fill_style;
  draw_shape(ctx, shape);
}

void draw_line(
    Context* ctx,
    vec2 from,
    vec2 to,
    StrokeStyle stroke_style) {
  DrawCommand shape;
  shape.path.moveTo(from.x, from.y);
  shape.path.lineTo(to.x, to.y);
  shape.stroke_style = stroke_style;
  draw_shape(ctx, shape);
}

ReturnCode draw_text(
    Context* ctx,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    TextStyle text_style) {
  draw::text_op op;
  op.text = text;
  op.placement.position = position;
  op.placement.align_x = align_x;
  op.placement.align_y = align_y;
  op.text_style = text_style;
  op.draw_style.fill_solid.push_back(draw_style::fill_solid(text_style.color));

  if (rotate) {
    op.transform = mul(
        translate2({position.x, position.y}),
        mul(
            rotate2(rotate),
            translate2({-position.x, -position.y})));
  }

  return draw::text(ctx, op);
}

ReturnCode draw_text(
    Context* ctx,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    TextStyle text_style) {
  return draw_text(
      ctx,
      text,
      position,
      align_x,
      align_y,
      0,
      text_style);
}

} // namespace clip


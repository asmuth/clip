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

namespace clip {

void draw_shape(Context* ctx, DrawCommand elem) {
  layer_get(ctx)->drawlist.emplace_back(std::move(elem));
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

void draw_text(Context* ctx, const TextInfo& elem) {
  const auto& style = elem.style;

  for (const auto& gg : elem.glyphs) {
    for (const auto& g : gg.glyphs) {
      auto gt = translate2({g.x, g.y});
      if (elem.transform) {
        gt = mul(*elem.transform, gt);
      }

      Path gp;
      auto rc = font_get_glyph_path(
          g.font,
          elem.style.font_size,
          layer_get_dpi(ctx),
          g.codepoint,
          &gp);

      if (!rc) {
        return;
      }

      DrawCommand shape;
      shape.path = path_transform(gp, gt);
      shape.fill_style.color = style.color;
      draw_shape(ctx, shape);
    }
  }

}

ReturnCode draw_text(
    Context* ctx,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    TextStyle style) {
  const auto layer = layer_get(ctx);

  convert_unit_typographic(layer_get_dpi(ctx), layer_get_rem(ctx), &style.font_size);

  text::TextSpan span;
  span.text_direction = style.direction;
  span.text = text;
  span.font = style.font;
  span.font_size = style.font_size;
  span.span_id = 0;
  span.script = style.default_script;
  span.language = style.default_language;

  if (span.script.empty()) {
    span.script = layer->text_default_script;
  }

  if (span.language.empty()) {
    span.language = layer->text_default_language;
  }

  text::TextLine line;
  line.spans.push_back(span);
  line.base_direction = style.direction;

  if (auto rc = text_reorder_bidi_line(&line); !rc) {
    return ERROR;
  }

  Rectangle bbox;
  std::vector<text::GlyphPlacementGroup> glyphs;
  if (auto rc = text::text_layout_line(line, layer_get_dpi(ctx), &glyphs, &bbox); !rc) {
    return rc;
  }

  auto offset = layout_align(bbox, position, align_x, align_y);

  for (auto& gg : glyphs) {
    for (auto& g : gg.glyphs) {
      g.x += offset.x;
      g.y += offset.y;
    }
  }

  TextInfo op;
  op.text = text;
  op.glyphs = std::move(glyphs);

  if (rotate) {
    op.transform = mul(
        translate2({position.x, position.y}),
        mul(
            rotate2(rotate),
            translate2({-position.x, -position.y})));
  }

  op.style = style;
  op.origin = offset;
  draw_text(ctx, op);
  return OK;
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


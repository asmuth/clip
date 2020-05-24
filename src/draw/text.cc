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
#include "text.h"
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "context.h"
#include "color_reader.h"
#include "style_reader.h"
#include "style_util.h"
#include "typographic_map.h"
#include "typographic_reader.h"
#include "layout.h"
#include "marker.h"
#include "scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/layout.h"
#include "graphics/text.h"
#include "graphics/text_shaper.h"
#include "graphics/text_layout.h"
#include "graphics/text_support.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::draw {

ReturnCode text(Context* ctx, const text_op& op) {
  const auto& style = op.text_style;
  const auto& layer = layer_get(ctx);
  auto dpi = layer_get_dpi(layer);
  auto font_size_pt = unit_to_pt(op.text_style.font_size, dpi);

  /* convert the text to glyphs placements (layout) */
  text::TextSpan span;
  span.text_direction = style.direction;
  span.text = op.text;
  span.font = op.text_style.font;
  span.font_size = op.text_style.font_size;
  span.span_id = 0;
  span.script = op.text_style.default_script;
  span.language = op.text_style.default_language;

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

  /* draw the glyphs */
  auto offset = layout_align(
      bbox,
      {
        op.placement.position.x,
        op.placement.position.y,
      },
      op.placement.align_x,
      op.placement.align_y);

  for (const auto& gg : glyphs) {
    for (const auto& g : gg.glyphs) {
      // FIXME: offset here
      auto gt = translate2({g.x + offset.x, g.y + offset.y});

      if (op.transform) {
        gt = mul(*op.transform, gt);
      }

      Path gp;
      auto rc = font_get_glyph_path(
          g.font,
          font_size_pt,
          dpi,
          g.codepoint,
          &gp);

      if (!rc) {
        return rc;
      }

      DrawCommand shape;
      shape.path = path_transform(gp, gt);
      shape.style = op.draw_style;
      draw_shape(ctx, shape);
    }
  }

  return OK;
}

ReturnCode text(
    Context* ctx,
    const std::string& text,
    const TextPlacement& placement,
    TextStyle style) {
  text_op op;
  op.text = text;
  op.placement = placement;
  op.text_style = style;
  op.draw_style.fill_solid.push_back(draw_style::fill_solid(style.color));
  return draw::text(ctx, op);
}

ReturnCode text(
    Context* ctx,
    const Expr* expr) {
  const auto layer = layer_get(ctx);

  text_op op;
  op.text_style.font = layer_get_font(ctx);
  op.text_style.font_size = layer_get_font_size(ctx);

  op.placement.position[0] = layer_get_width(*layer).value * .5f;
  op.placement.position[1] = layer_get_height(*layer).value * .5f;
  op.placement.align_x = HAlign::CENTER;
  op.placement.align_y = VAlign::CENTER;

  /* read arguments */
  auto config_rc = expr_walk_map(expr, {
    {
      "position",
      std::bind(
          &expr_to_vec2,
          _1,
          layer_get_uconv_width(*layer),
          layer_get_uconv_height(*layer),
          &op.placement.position)
    },
    {"text", std::bind(&expr_to_string, _1, &op.text)},
    {"font", expr_call_string_fn(std::bind(&font_load_best, _1, &op.text_style.font))},
    {"font-size", std::bind(&expr_to_font_size, _1, *layer, &op.text_style.font_size)},
    {"color", std::bind(&style_read_color, _1, *layer, &op.draw_style)},
    {"fill", std::bind(&style_read_fill, _1, *layer, &op.draw_style)},
    {"stroke", std::bind(&style_read_stroke, _1, *layer, &op.draw_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  if (!style_is_visible(op.draw_style)) {
    op.draw_style = layer->draw_default_style;
  }

  return text(ctx, op);
}

} // namespace clip::draw


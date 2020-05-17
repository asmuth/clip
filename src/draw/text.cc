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

ReturnCode text_draw(
    Context* ctx,
    const std::string& text,
    const TextPlacement& placement,
    TextStyle style) {
  const auto layer = layer_get(ctx);

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

  auto offset = layout_align(
      bbox,
      {
        placement.position[0].value,
        placement.position[1].value,
      },
      placement.align_x,
      placement.align_y);

  for (auto& gg : glyphs) {
    for (auto& g : gg.glyphs) {
      g.x += offset.x;
      g.y += offset.y;
    }
  }

  TextInfo op;
  op.text = text;
  op.glyphs = std::move(glyphs);
  op.style = style;
  op.origin = offset;
  draw_text(ctx, op);
  return OK;
}

ReturnCode text_draw(
    Context* ctx,
    const Expr* expr) {
  const auto layer = layer_get(ctx);

  std::string text;
  TextPlacement placement;
  TextStyle style;
  style.font = layer_get_font(ctx);
  style.font_size = layer_get_font_size(ctx);

  placement.position[0] = layer_get_width(*layer) * .5f;
  placement.position[1] = layer_get_height(*layer) * .5f;
  placement.align_x = HAlign::CENTER;
  placement.align_y = VAlign::CENTER;

  /* read arguments */
  auto config_rc = expr_walk_map(expr, {
    {
      "position",
      std::bind(
          &expr_to_vec2,
          _1,
          layer_get_uconv_width(*layer),
          layer_get_uconv_height(*layer),
          &placement.position)
    },
    {"color", std::bind(&color_read, ctx, _1, &style.color)},
    {"text", std::bind(&expr_to_string, _1, &text)},
    {"font", expr_call_string_fn(std::bind(&font_load_best, _1, &style.font))},
    {"font-size", std::bind(&measure_read, _1, &style.font_size)},
  });

  if (!config_rc) {
    return config_rc;
  }

  return text_draw(ctx, text, placement, style);
}

} // namespace clip::draw


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
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "typographic_reader.h"
#include "color_reader.h"
#include "style_reader.h"

using namespace std::placeholders;

namespace clip {

Context::Context() :
    width(from_px(1024)),
    height(from_px(512)),
    dpi(96),
    font_defaults(true),
    background_color(Color::fromRGB(1, 1, 1)),
    foreground_color(Color::fromRGB(0, 0, 0)),
    text_color(Color::fromRGB(0, 0, 0)),
    font_size(from_pt(11)),
    color_palette(color_palette_default()) {}

ReturnCode context_setup_defaults(Context* ctx) {
  if (ctx->font_defaults) {
    if (auto rc = font_load_defaults(&ctx->font); !rc) {
      return rc;
    }
  }

  for (const auto& f : ctx->font_load) {
    if (auto rc = font_load_best(f, &ctx->font); !rc) {
      return rc;
    }
  }

  return OK;
}

Rectangle context_get_clip(const Context* ctx) {
  if (ctx->layout_stack.empty()) {
    auto margins = ctx->margins;
    for (auto& m : margins) {
      convert_unit_typographic(ctx->dpi, context_get_rem(ctx), &m);
    }

    return layout_margin_box(
        Rectangle(0, 0, ctx->width, ctx->height),
        margins[0],
        margins[1],
        margins[2],
        margins[3]);
  } else {
    return ctx->layout_stack.back();
  }
}

Measure context_get_rem(const Context* ctx) {
  auto rem_default = from_px(16);
  auto rem = ctx->font_size;
  convert_unit_typographic(ctx->dpi, rem_default, &rem);
  return rem;
}

ReturnCode context_set_background(
    Context* ctx,
    const Expr* expr) {
  Rectangle rect = context_get_clip(ctx);
  FillStyle fill_style;
  StrokeStyle stroke_style;
  stroke_style.line_width = from_pt(1);

  /* read arguments */
  auto config_rc = expr_walk_map(expr, {
    {
      "color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &stroke_style.color),
        bind(&fill_style_read_solid, ctx, _1, &fill_style),
      })
    },
    {"fill", bind(&fill_style_read, ctx, _1, &fill_style)},
    {"stroke-color", bind(&color_read, ctx, _1, &stroke_style.color)},
    {"stroke-width", bind(&measure_read, _1, &stroke_style.line_width)},
    {"stroke-style", bind(&stroke_style_read, ctx, _1, &stroke_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  Path p;
  path_add_rectangle(&p, rect);
  draw_path(ctx, p, stroke_style, fill_style);

  return OK;
}

} // namespace clip


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

ReturnCode context_configure(Context* ctx, const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() < 2) {
    return error(ERROR, "Expected at least two arguments");
  }

  if (expr_is_value(args[0], "width")) {
    return measure_read(args[1], &ctx->width);
  }

  if (expr_is_value(args[0], "height")) {
    return measure_read(args[1], &ctx->height);
  }

  if (expr_is_value(args[0], "dpi")) {
    return expr_to_float64(args[1], &ctx->dpi);
  }

  return errorf(ERROR, "Unknown command: {}", expr_get_value(args[0]));
}

ReturnCode context_set_default(Context* ctx, const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() != 3) {
    return error(ERROR, "Expected exactly two arguments");
  }

  if (!expr_is_value_literal(args[1])) {
    return error(ERROR, "The first argument to 'default' must be a string literal.");
  }

  ctx->defaults[expr_get_value(args[1])] = expr_clone(args[2]);
  return OK;
}

Rectangle context_get_clip(const Context* ctx) {
  if (ctx->layout_stack.empty()) {
    return Rectangle(0, 0, ctx->width, ctx->height);
  } else {
    return ctx->layout_stack.back();
  }
}

} // namespace clip



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

namespace clip {

Context::Context() :
    width(1024),
    height(512),
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
    return Rectangle(0, 0, 0, 0);
  } else {
    return ctx->layout_stack.back();
  }
}

} // namespace clip



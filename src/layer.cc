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
#include "layer.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "typographic_reader.h"
#include "color_reader.h"
#include "style_reader.h"

using namespace std::placeholders;

namespace clip {

Layer::Layer() :
    width(from_px(1024)),
    height(from_px(512)),
    dpi(96),
    background_color(Color::fromRGB(1, 1, 1)),
    foreground_color(Color::fromRGB(0, 0, 0)),
    text_color(Color::fromRGB(0, 0, 0)),
    font_size(from_pt(11)),
    color_palette(color_palette_default()) {}

ReturnCode layer_create(
    Context* ctx,
    std::unique_ptr<Layer>* layer_storage) {
  auto layer = new Layer();
  layer_storage->reset(layer);

  if (ctx->font_defaults) {
    if (auto rc = font_load_defaults(&layer->font); !rc) {
      return rc;
    }
  }

  for (const auto& f : ctx->font_load) {
    if (auto rc = font_load_best(f, &layer->font); !rc) {
      return rc;
    }
  }

  return OK;
}

Layer* layer_get(Context* ctx) {
  return ctx->layer.get();
}

const Layer* layer_get(const Context* ctx) {
  return ctx->layer.get();
}

float layer_get_dpi(const Context* ctx) {
  return layer_get_dpi(layer_get(ctx));
}

float layer_get_dpi(const Layer* layer) {
  return layer->dpi;
}

Measure layer_get_rem(const Context* ctx) {
  return layer_get_rem(layer_get(ctx));
}

Measure layer_get_rem(const Layer* ctx) {
  auto rem_default = from_px(16);
  auto rem = ctx->font_size;
  convert_unit_typographic(ctx->dpi, rem_default, &rem);
  return rem;
}

const FontInfo& layer_get_font(const Context* ctx) {
  return layer_get_font(layer_get(ctx));
}

const FontInfo& layer_get_font(const Layer* layer) {
  return layer->font;
}

Measure layer_get_font_size(const Context* ctx) {
  return layer_get_font_size(layer_get(ctx));
}

Measure layer_get_font_size(const Layer* layer) {
  return layer->font_size;
}

} // namespace clip


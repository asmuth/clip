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

void layer_resize(
    Layer* layer,
    Measure width,
    Measure height) {
  layer->width = width;
  layer->height = height;
}

ReturnCode layer_resize(
    Context* ctx,
    const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() != 2) {
    return err_invalid_nargs(args.size(), 2);
  }

  Measure width;
  if (auto rc = measure_read(args[0], &width); !rc) {
    return rc;
  }

  Measure height;
  if (auto rc = measure_read(args[1], &height); !rc) {
    return rc;
  }

  layer_resize(layer_get(ctx), width, height);
  return OK;
}

Layer* layer_get(Context* ctx) {
  return ctx->layer.get();
}

const Layer* layer_get(const Context* ctx) {
  return ctx->layer.get();
}

double layer_get_dpi(const Layer* layer) {
  return layer->dpi;
}

double layer_get_dpi(const Context* ctx) {
  return layer_get_dpi(layer_get(ctx));
}

void layer_set_dpi(
    Layer* layer,
    double dpi) {
  layer->dpi = dpi;
}

ReturnCode layer_set_dpi(
    Context* ctx,
    const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() != 1) {
    return err_invalid_nargs(args.size(), 1);
  }

  double dpi;
  if (auto rc = expr_to_float64(args[0], &dpi); !rc) {
    return rc;
  }

  layer_set_dpi(layer_get(ctx), dpi);
  return OK;
}

Measure layer_get_rem(const Layer* ctx) {
  auto rem_default = from_px(16);
  auto rem = ctx->font_size;
  convert_unit_typographic(ctx->dpi, rem_default, &rem);
  return rem;
}

Measure layer_get_rem(const Context* ctx) {
  return layer_get_rem(layer_get(ctx));
}


const FontInfo& layer_get_font(const Layer* layer) {
  return layer->font;
}

const FontInfo& layer_get_font(const Context* ctx) {
  return layer_get_font(layer_get(ctx));
}

void layer_set_font(
    Layer* layer,
    FontInfo font) {
  layer->font = std::move(font);
}

ReturnCode layer_set_font(
    Context* ctx,
    const Expr* expr) {
  auto layer = layer_get(ctx);

  auto args = expr_collect(expr);
  if (args.size() != 1) {
    return err_invalid_nargs(args.size(), 1);
  }

  FontInfo font;
  if (auto rc = expr_call_string(args[0], std::bind(&font_load_best, _1, &layer->font)); !rc) {
    return rc;
  }

  return OK;
}

Measure layer_get_font_size(const Layer* layer) {
  return layer->font_size;
}

Measure layer_get_font_size(const Context* ctx) {
  return layer_get_font_size(layer_get(ctx));
}

void layer_set_font_size(
    Layer* layer,
    Measure font_size) {
  layer->font_size = font_size;
}

ReturnCode layer_set_font_size(
    Context* ctx,
    const Expr* expr) {
  auto args = expr_collect(expr);
  if (args.size() != 1) {
    return err_invalid_nargs(args.size(), 1);
  }

  Measure font_size;
  if (auto rc = measure_read(args[0], &font_size); !rc) {
    return rc;
  }

  layer_set_font_size(layer_get(ctx), font_size);
  return OK;
}

} // namespace clip


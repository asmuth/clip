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

ReturnCode layer_resize(
    Layer* layer,
    Measure width,
    Measure height) {
  layer->width = width;
  layer->height = height;
  return OK;
}

ReturnCode layer_resize(
    Layer* layer,
    const std::string& size) {
  static const std::unordered_map<std::string, std::tuple<Measure, Measure>> sizes = {
    {"A0", {from_mm(841), from_mm(1189)}},
    {"A0*", {from_mm(1189), from_mm(841)}},
    {"A1", {from_mm(594), from_mm(841)}},
    {"A1*", {from_mm(841), from_mm(594)}},
    {"A2", {from_mm(420), from_mm(594)}},
    {"A2*", {from_mm(594), from_mm(420)}},
    {"A3", {from_mm(297), from_mm(420)}},
    {"A3*", {from_mm(420), from_mm(297)}},
    {"A4", {from_mm(210), from_mm(297)}},
    {"A4*", {from_mm(297), from_mm(210)}},
    {"A5", {from_mm(148), from_mm(210)}},
    {"A5*", {from_mm(210), from_mm(148)}},
    {"A6", {from_mm(105), from_mm(148)}},
    {"A6*", {from_mm(148), from_mm(105)}},
  };

  const auto& size_iter = sizes.find(size);
  if (size_iter == sizes.end()) {
    std::vector<std::string> size_names;
    for (const auto& s : sizes) {
      size_names.push_back(s.first);
    }

    return err_invalid_value(size, size_names);
  }

  layer->width = std::get<0>(size_iter->second);
  layer->height = std::get<1>(size_iter->second);

  return OK;
}

ReturnCode layer_resize(
    Context* ctx,
    const Expr* expr) {
  auto args = expr_collect(expr);

  switch (args.size()) {
    case 1: {
      return layer_resize(layer_get(ctx), expr_get_value(args[0]));
    }

    case 2: {
      Measure width;
      if (auto rc = measure_read(args[0], &width); !rc) {
        return rc;
      }

      Measure height;
      if (auto rc = measure_read(args[1], &height); !rc) {
        return rc;
      }

      return layer_resize(layer_get(ctx), width, height);
    }

    default:
      return err_invalid_nargs(args.size(), 2);
  }
}

Layer* layer_get(Context* ctx) {
  return ctx->layer.get();
}

const Layer* layer_get(const Context* ctx) {
  return ctx->layer.get();
}

Number layer_get_width(const Layer& layer) {
  switch (layer.width.unit) {
    case Unit::MM:
      return unit_from_mm(layer.width.value, layer.dpi);
    case Unit::PT:
      return unit_from_pt(layer.width.value, layer.dpi);
    case Unit::PX:
      return unit_from_px(layer.width.value);
    default:
      return 0;
  }
}

Number layer_get_height(const Layer& layer) {
  switch (layer.height.unit) {
    case Unit::MM:
      return unit_from_mm(layer.height.value, layer.dpi);
    case Unit::PT:
      return unit_from_pt(layer.height.value, layer.dpi);
    case Unit::PX:
      return unit_from_px(layer.height.value);
    default:
      return 0;
  }
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

UnitConvMap layer_get_uconv_width(const Layer& layer) {
  auto width = layer_get_width(layer).value;

  UnitConvMap conv;
  conv[Unit::MM] = std::bind(&unit_from_mm, _1, layer.dpi);
  conv[Unit::PT] = std::bind(&unit_from_pt, _1, layer.dpi);
  conv[Unit::PX] = std::bind(&unit_from_px, _1);
  conv[Unit::PERCENT] = std::bind(&unit_from_percent, _1, width);
  return conv;
}

UnitConvMap layer_get_uconv_height(const Layer& layer) {
  auto height = layer_get_height(layer).value;

  UnitConvMap conv;
  conv[Unit::MM] = std::bind(&unit_from_mm, _1, layer.dpi);
  conv[Unit::PT] = std::bind(&unit_from_pt, _1, layer.dpi);
  conv[Unit::PX] = std::bind(&unit_from_px, _1);
  conv[Unit::PERCENT] = std::bind(&unit_from_percent, _1, height);
  return conv;
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


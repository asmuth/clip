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
#pragma once
#include "return_code.h"
#include "graphics/draw.h"
#include "graphics/geometry.h"
#include "color_palette.h"
#include "scale.h"

namespace clip {

struct Layer {
  Layer();

  Measure width;
  Measure height;
  double dpi;

  FontInfo font;
  Measure font_size;

  std::string text_default_script;
  std::string text_default_language;

  Color background_color;
  Color foreground_color;
  Color text_color;
  ColorPalette color_palette;

  DrawCommandList drawlist;
};

ReturnCode layer_create(
    Context* ctx,
    std::unique_ptr<Layer>* layer_storage);

void layer_resize(
    Layer* layer,
    Measure width,
    Measure height);

ReturnCode layer_resize(
    Context* ctx,
    const Expr* expr);

void layer_set_dpi(
    Layer* layer,
    double dpi);

ReturnCode layer_set_dpi(
    Context* ctx,
    const Expr* expr);

Layer* layer_get(Context* ctx);
const Layer* layer_get(const Context* ctx);

double layer_get_dpi(const Context* ctx);
double layer_get_dpi(const Layer* layer);

Measure layer_get_rem(const Context* ctx);
Measure layer_get_rem(const Layer* layer);

const FontInfo& layer_get_font(const Context* ctx);
const FontInfo& layer_get_font(const Layer* layer);

Measure layer_get_font_size(const Context* ctx);
Measure layer_get_font_size(const Layer* layer);

} // namespace clip



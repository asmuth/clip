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

  draw_style::compound draw_default_style;
  DrawCommandList drawlist;
};

ReturnCode layer_create(
    Context* ctx,
    std::unique_ptr<Layer>* layer_storage);

ReturnCode layer_resize(
    Layer* layer,
    Measure width,
    Measure height);

ReturnCode layer_resize(
    Layer* layer,
    const std::string& size);

ReturnCode layer_resize_cmd(
    Context* ctx,
    const Expr* expr);

Layer* layer_get(Context* ctx);
const Layer* layer_get(const Context* ctx);

Number layer_get_width(const Layer& layer);
Number layer_get_height(const Layer& layer);

double layer_get_dpi(const Layer& layer);
double layer_get_dpi(const Layer* layer);
double layer_get_dpi(const Context* ctx);
void layer_set_dpi(Layer* layer, double dpi);
ReturnCode layer_set_dpi_cmd(Context* ctx, const Expr* expr);

UnitConvMap layer_get_uconv_width(const Layer& layer);
UnitConvMap layer_get_uconv_height(const Layer& layer);
UnitConvMap layer_get_uconv_size(const Layer& layer);
UnitConvMap layer_get_uconv_font_size(const Layer& layer);

const FontInfo& layer_get_font(const Layer* layer);
const FontInfo& layer_get_font(const Context* ctx);
FontInfo layer_set_font(const Layer* layer, FontInfo font);
ReturnCode layer_set_font_cmd(Context* ctx, const Expr* expr);

Number layer_get_font_size(const Layer& layer);
Number layer_get_font_size(const Layer* layer);
Number layer_get_font_size(const Context* ctx);
void layer_set_font_size(const Layer* layer, Measure font_size);
ReturnCode layer_set_font_size_cmd(Context* ctx, const Expr* expr);


} // namespace clip



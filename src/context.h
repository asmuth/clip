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

struct Context {
  Context();

  Measure width;
  Measure height;
  double dpi;

  bool font_defaults;
  std::vector<std::string> font_load;
  FontInfo font;
  Measure font_size;

  std::string text_default_script;
  std::string text_default_language;

  ColorPalette color_palette;
  Color background_color;
  Color foreground_color;
  Color text_color;

  std::array<Measure, 4> margins;
  std::vector<Rectangle> layout_stack;

  DrawCommandList drawlist;

  ScaleConfig scale_x;
  ScaleConfig scale_y;
};

ReturnCode context_setup_defaults(Context* ctx);

Rectangle context_get_clip(const Context* ctx);

ReturnCode context_set_background(Context* ctx, const Expr* expr);

} // namespace clip



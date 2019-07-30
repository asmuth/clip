/**
 * This file is part of the "fviz" project
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
#include <memory>
#include "return_code.h"
#include "color_palette.h"
#include "graphics/measure.h"
#include "graphics/color.h"
#include "graphics/text.h"
#include "sexpr.h"
#include "element_factory.h"

namespace fviz {
class Layer;

struct Environment {
  Environment();
  Measure screen_width;
  Measure screen_height;
  double dpi;
  bool font_defaults;
  std::vector<std::string> font_load;
  FontInfo font;
  Measure font_size;
  ColorPalette color_palette;
  Color background_color;
  Color foreground_color;
  Color text_color;
  std::string text_default_script;
  std::string text_default_language;
  std::array<Measure, 4> margins;
  ElementMap element_map;
};

ReturnCode environment_setup_defaults(Environment* env);

ReturnCode environment_configure(Environment* env, const Expr* e);

} // namespace fviz



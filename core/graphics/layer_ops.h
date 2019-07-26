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
#include <stdlib.h>
#include <vector>
#include <string>
#include <variant>

#include "color.h"
#include "measure.h"
#include "brush.h"
#include "text.h"

namespace fviz {
namespace layer_ops {

struct SubmitOp {};

struct BrushStrokeOp {
  Rectangle clip;
  Path path;
  StrokeStyle style;
};

struct BrushFillOp {
  Rectangle clip;
  Path path;
  FillStyle style;
};

struct TextSpanOp {
  std::string text;
  std::vector<text::GlyphPlacement> glyphs;
  Point origin;
  double rotate;
  Point rotate_pivot;
  TextStyle style;
};

using Op = std::variant<
    SubmitOp,
    BrushFillOp,
    BrushStrokeOp,
    TextSpanOp>;

} // namespace layer_ops
} // namespace fviz


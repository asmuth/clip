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
#include "sexpr.h"
#include "graphics/path.h"
#include "graphics/color.h"
#include "graphics/measure.h"
#include "utils/option.h"

namespace clip {
struct Page;

enum class AntialiasingMode {
  ENABLE, DISABLE
};

/**
 * The FillStyle controls how shapes are filled in
 */
struct FillStyle {
  FillStyle();

  Option<Color> color;
  bool hatch;
  double hatch_angle_deg;
  double hatch_offset;
  double hatch_stride;
  double hatch_width;
};

/**
 * The StrokeStyle controls how shapes are stroked
 */
struct StrokeStyle {
  enum DashType {
    SOLID,
    DASH
  };

  StrokeStyle();

  Measure line_width;
  Color color;
  DashType dash_type;
  Measure dash_offset;
  std::vector<Measure> dash_pattern;
};

} // namespace clip


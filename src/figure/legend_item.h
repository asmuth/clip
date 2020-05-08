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
#include "context.h"
#include "marker.h"

namespace clip::plotgen {

struct LegendItem {
  std::string label;
  HAlign label_align;
  Measure label_margin;
  Color label_color;
  FontInfo label_font;
  Measure label_font_size;
  Marker marker;
  HAlign marker_align;
  Measure marker_margin;
  Color marker_color;
  Measure marker_size;
};

ReturnCode legend_item_configure(
    Context* ctx,
    const Expr* expr,
    LegendItem* config);

ReturnCode legend_item_calculate_size(
    Context* ctx,
    LegendItem* config,
    double* min_width,
    double* min_height);

ReturnCode legend_item_draw(
    Context* ctx,
    LegendItem* config,
    const Rectangle& bbox);

} // namespace clip::plotgen


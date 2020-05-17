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
#include "sexpr.h"
#include "return_code.h"

namespace clip {

struct PlotConfig {
  ScaleConfig scale_x;
  ScaleConfig scale_y;

  std::array<Number, 4> margins;
  std::vector<Rectangle> layout_stack;
};

ReturnCode plot_eval(
    Context* ctx,
    const Expr* expr);

Rectangle plot_get_clip(const PlotConfig* plot, const Layer* layer);

ReturnCode plot_set_background(
    Context* ctx,
    const PlotConfig* plot,
    const Expr* expr);

} // namespace clip


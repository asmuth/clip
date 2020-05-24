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

#include <graphics/brush.h>
#include "style.h"
#include "return_code.h"
#include "sexpr.h"

namespace clip {

ReturnCode stroke_style_read(
    const Context* ctx,
    const Expr* expr,
    StrokeStyle* style);

ReturnCode fill_style_read(
    const Context* ctx,
    const Expr* expr,
    FillStyle* style);

ReturnCode fill_style_read_solid(
    const Context* ctx,
    const Expr* expr,
    FillStyle* style);

ReturnCode style_read(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles);

ReturnCode style_read_fill(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles);

ReturnCode style_read_stroke(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles);

ReturnCode style_read_color(
    const Expr* expr,
    const Layer& layer,
    draw_style::compound* styles);

} // namespace clip


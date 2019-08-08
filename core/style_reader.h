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

#include <graphics/brush.h>
#include "return_code.h"
#include "sexpr.h"

namespace fviz {

ReturnCode stroke_style_read(
    const Environment& env,
    const Expr* expr,
    StrokeStyle* style);

ReturnCode fill_style_read(
    const Environment& env,
    const Expr* expr,
    FillStyle* style);

ReturnCode fill_style_read_solid(
    const Environment& env,
    const Expr* expr,
    FillStyle* style);

} // namespace fviz


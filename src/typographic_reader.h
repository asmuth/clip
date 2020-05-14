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
#include "graphics/measure.h"
#include "typographic_map.h"
#include "return_code.h"
#include "sexpr.h"

namespace clip {

ReturnCode measure_read(
    const Expr* expr,
    Measure* value);

ReturnCode measure_readn(
    const Context* ctx,
    const Expr* expr,
    Measure* value);

ReturnCode measure_read_opt(
    const Expr* expr,
    Option<Measure>* value);

ReturnCode measure_read_list(
    const Expr* expr,
    std::vector<Measure>* measures);

ReturnCode measure_map_read(
    const Context* ctx,
    const Expr* expr,
    MeasureMap* measure_map);

} // namespace clip


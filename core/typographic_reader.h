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
#include "graphics/measure.h"
#include "return_code.h"
#include "sexpr.h"

namespace fviz {

ReturnCode measure_read(
    const Expr* expr,
    Measure* value);

ReturnCode measure_read_opt(
    const Expr* expr,
    std::optional<Measure>* value);

ReturnCode measure_read_list(
    const Expr* expr,
    std::vector<Measure>* measures);

} // namespace fviz


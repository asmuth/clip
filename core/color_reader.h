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
#include "environment.h"
#include "graphics/color.h"
#include "return_code.h"
#include "sexpr.h"

namespace fviz {

ReturnCode color_read(
    const Environment& env,
    const Expr* expr,
    Color* value);

ReturnCode color_read_opt(
    const Environment& env,
    const Expr* expr,
    std::optional<Color>* var);

} // namespace fviz


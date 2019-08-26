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
#include <unordered_map>
#include <optional>

#include "graphics/color.h"
#include "graphics/page_description.h"
#include "graphics/geometry.h"
#include "return_code.h"
#include "sexpr.h"

namespace clip {

using Arrow = std::function<
    ReturnCode (
        const Point& from,
        const Point& to,
        const Measure& size,
        const Color& color,
        Page* page)>;

Arrow arrow_create_default();

} // namespace clip


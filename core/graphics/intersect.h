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
#include <unordered_map>
#include <optional>

#include "graphics/color.h"
#include "graphics/layer.h"
#include "graphics/geometry.h"
#include "return_code.h"
#include "sexpr.h"

namespace fviz {

/**
 * Given two (infinite) lines defined by a point on each of the the lines p0, p1
 * and their normal direction vectors v0, v1, compute the point of intersection.
 * Returns false if the two lines are parallel.
 */
bool intersect_line_line(
    vec2 p0,
    vec2 v0,
    vec2 p1,
    vec2 v1,
    vec2* p = nullptr);

} // namespace fviz


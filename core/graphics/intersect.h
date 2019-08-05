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
#include "graphics/polygon.h"
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

/**
 * Given an infinite line defined by a point on the line and a direction vector
 * and a line segment defined by its start and end point, compute their point of
 * intersection if any. Returns false if the two lines are parallel.
 */
bool intersect_line_lineseg(
    vec2 p0,
    vec2 v0,
    vec2 s1,
    vec2 e1,
    vec2* p = nullptr);

/**
 * Given two line sgements defined by the two start and end points of the two
 * line segments s0, s1 and e1, e2, compute their point of intersection if any.
 * Returns false if the two line segements are parallel or coincident.
 */
bool intersect_lineseg_lineseg(
    vec2 s0,
    vec2 e0,
    vec2 s1,
    vec2 e1,
    vec2* p = nullptr);

/**
 * Given a polygon and an (infinite) line defined by a point on the line p1 and
 * the lines direction v1, compute their points of intersection if any.
 */
void intersect_poly_line(
    const Polygon2& p0,
    const vec2& p1,
    const vec2& v1,
    std::vector<vec2>* x);

/**
 * Given a polygon and a line segement defined by its start and end points of the
 * two line segments s1, e1, compute their points of intersection if any.
 */
void intersect_poly_lineseg(
    const Polygon2& p0,
    const vec2& s1,
    const vec2& e1,
    std::vector<vec2>* x);

} // namespace fviz


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
#include "geometry.h"

namespace clip {

enum class HAlign {
  LEFT, CENTER, RIGHT
};

enum class VAlign {
  TOP, CENTER, BOTTOM
};

enum class Direction {
  HORIZONTAL, VERTICAL
};

Rectangle layout_margin_box(
    const Rectangle& parent,
    double margin_top,
    double margin_right,
    double margin_bottom,
    double margin_left);

/**
 * Given a bounding box and a target point (tx, ty) and (sx, sy) == (0, 0),
 * calculate a translate offset (ox, oy) for the bounding box so that new
 * bounding box midpoint point is equal to the target point.
 */
Point layout_align(
    const Rectangle& bbox,
    const Point& target);

/**
 * Given a bounding box and a target point (tx, ty) and (sx, sy) == (0, 0),
 * calculate a translate offset (ox, oy) for the bounding box so that new
 * bounding box alignment point is equal to the target point. You can choose
 * from one of 9 alignment points using the halign and valign parameters.
 */
Point layout_align(
    const Rectangle& bbox,
    Point target,
    HAlign halign,
    VAlign valign);


/**
 * Return the smallest bounding box that can contain the input box after it
 * was rotated `angle_deg` around it's center point
 */
Rectangle box_rotate_bounds(
    const Rectangle& bbox,
    double angle_deg);

} // namespace clip


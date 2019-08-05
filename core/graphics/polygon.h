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
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include "geometry.h"

namespace fviz {

/**
 * A polygon in 2D space defined by its vertices. A polygon is always closed;
 * for N vertices in the polygon, the polygon has N edges where one edge goes
 * from the last vertex in the list to the first vertex in the list.
 *
 * Note that polygons can be given in clockwise or counter-clockwise winding
 * order and that they may or may not self-intersect. Methods that work on
 * polygons will usually accept only a subset of these cases.
 */
struct Polygon2 {
  std::vector<vec2> vertices;
};

std::ostream& operator <<(std::ostream& os, const Polygon2& p);

} // namespace fviz


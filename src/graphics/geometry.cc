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
#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>
#include "layout.h"

namespace clip {

Rectangle::Rectangle() :
    x(0.0f),
    y(0.0f),
    w(0.0f),
    h(0.0f) {}

Rectangle::Rectangle(
    double _x,
    double _y,
    double _w,
    double _h) :
    x(_x),
    y(_y),
    w(_w),
    h(_h) {}

std::ostream& operator <<(std::ostream& os, const Rectangle& r) {
  os << "Rectangle(";
  os << r.x;
  os << ", ";
  os << r.y;
  os << ", ";
  os << r.w;
  os << ", ";
  os << r.h;
  os << ")";
  return os;
}

PolyLine2 polyline3_to_polyline2(const PolyLine3& p) {
  PolyLine2 p2;

  for (const auto& x : p.vertices) {
    p2.vertices.emplace_back(x);
  }

  return p2;
}

Poly2 poly3_to_poly2(const Poly3& p) {
  Poly2 p2;
  p2.boundary = polyline3_to_polyline2(p.boundary);

  for (const auto& x : p.holes) {
    p2.holes.emplace_back(polyline3_to_polyline2(x));
  }

  return p2;
}

} // namespace clip


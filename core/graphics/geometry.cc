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

} // namespace clip


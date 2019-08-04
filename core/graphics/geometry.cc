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
#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>
#include "layout.h"

namespace fviz {

Point::Point() :
    x(0.0f),
    y(0.0f) {}

Point::Point(
    double _x,
    double _y) :
    x(_x),
    y(_y) {}

double vec2_magnitude(const vec2& v) {
  return sqrt(v.x * v.x + v.y * v.y);
}

vec2 vec2_normalize(const vec2& v) {
  auto m = vec2_magnitude(v);
  return {v.x / m, v.y / m};
}

vec2 vec2_add(const vec2& a, const vec2& b) {
  return {a.x + b.x, a.y + b.y};
}

vec2 vec2_sub(const vec2& a, const vec2& b) {
  return {a.x - b.x, a.y - b.y};
}

vec2 vec2_mul(const vec2& v, double s) {
  return {v.x * s, v.y * s};
}

double vec2_dot(const vec2& a, const vec2& b) {
  return a.x * b.x + a.y * b.y;
}

vec2 vec2_from_deg(double deg) {
  double a = (deg - 180.0) / 180.0 * M_PI;
  return {cos(a), sin(a)};
}

std::ostream& operator <<(std::ostream& os, const Point& p) {
  os << "Point(";
  os << p.x;
  os << ", ";
  os << p.y;
  os << ")";
  return os;
}

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

} // namespace fviz


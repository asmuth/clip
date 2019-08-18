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

vec2::vec2() :
    x(0.0),
    y(0.0) {}

vec2::vec2(
    double _x,
    double _y) :
    x(_x),
    y(_y) {}

double vec2_magnitude(const vec2& v) {
  return sqrt(v.x * v.x + v.y * v.y);
}

vec2 normalize(const vec2& v) {
  auto m = vec2_magnitude(v);
  return {v.x / m, v.y / m};
}

vec2 add(const vec2& a, const vec2& b) {
  return {a.x + b.x, a.y + b.y};
}

vec2 sub(const vec2& a, const vec2& b) {
  return {a.x - b.x, a.y - b.y};
}

vec2 mul(const vec2& v, double s) {
  return {v.x * s, v.y * s};
}

double dot(const vec2& a, const vec2& b) {
  return a.x * b.x + a.y * b.y;
}

vec2 from_deg(double deg) {
  double a = -deg / 180.0 * M_PI;
  return {cos(a), sin(a)};
}

vec2 mean(const vec2* v, size_t v_len) {
  vec2 m;

  for (size_t i = 0; i < v_len; ++i) {
    m = add(m, v[i]);
  }

  return mul(m, 1.0 / v_len);
}

void sort_cw(vec2* v, size_t v_len) {
  auto m = mean(v, v_len);

  std::sort(v, v + v_len, [&m] (const auto& a, const auto& b) {
    auto da = sub(a, m);
    auto db = sub(b, m);
    return atan2(db.y, db.x) < atan2(da.y, da.x);
  });
}

void sort_ccw(vec2* v, size_t v_len) {
  auto m = mean(v, v_len);

  std::sort(v, v + v_len, [&m] (const auto& a, const auto& b) {
    auto da = sub(a, m);
    auto db = sub(b, m);
    return atan2(da.y, da.x) < atan2(db.y, db.x);
  });
}

std::ostream& operator <<(std::ostream& os, const vec2& p) {
  os << "vec2(";
  os << p.x;
  os << ", ";
  os << p.y;
  os << ")";
  return os;
}

} // namespace fviz


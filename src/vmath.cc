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

constexpr double TAU = M_PI * 2;

vec2::vec2() :
    x(0.0),
    y(0.0) {}

vec2::vec2(
    double _x,
    double _y) :
    x(_x),
    y(_y) {}

vec2::vec2(
    const vec3& v) :
    x(v.x),
    y(v.y) {}

double& vec2::operator[] (size_t idx) {
  static double invalid = std::nan("");

  switch (idx) {
    case 0: return x;
    case 1: return y;
    default: return invalid;
  }
}

double vec2::operator[] (size_t idx) const {
  switch (idx) {
    case 0: return x;
    case 1: return y;
    default: return std::nan("");
  }
}

vec3::vec3() :
    x(0.0),
    y(0.0),
    z(0.0) {}

vec3::vec3(
    double _x,
    double _y,
    double _z) :
    x(_x),
    y(_y),
    z(_z) {}

vec3::vec3(
    const vec2& v,
    double _z) :
    x(v.x),
    y(v.y),
    z(_z) {}

mat3::mat3() :
    a(0.0),
    b(0.0),
    c(0.0),
    d(0.0),
    e(0.0),
    f(0.0),
    g(0.0),
    h(0.0),
    i(0.0) {}

double magnitude(const vec2& v) {
  return sqrt(v.x * v.x + v.y * v.y);
}

vec2 normalize(const vec2& v) {
  auto m = magnitude(v);
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

mat3 mul(const mat3& a, const mat3& b) {
  mat3 m;

  m.a = a.a * b.a + a.b * b.d + a.c * b.g;
  m.b = a.a * b.b + a.b * b.e + a.c * b.h;
  m.c = a.a * b.c + a.b * b.f + a.c * b.i;
  m.d = a.d * b.a + a.e * b.d + a.f * b.g;
  m.e = a.d * b.b + a.e * b.e + a.f * b.h;
  m.f = a.d * b.c + a.e * b.f + a.f * b.i;
  m.g = a.g * b.a + a.h * b.d + a.i * b.g;
  m.h = a.g * b.b + a.h * b.e + a.i * b.h;
  m.i = a.g * b.c + a.h * b.f + a.i * b.i;

  return m;
}

vec3 mul(const mat3& m, const vec3& v) {
  return vec3(
      m.a * v.x + m.b * v.y + m.c * v.z,
      m.d * v.x + m.e * v.y + m.f * v.z,
      m.g * v.x + m.h * v.y + m.i * v.z);
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

mat3 rotate2(double angle) {
  double angle_tau = (-angle / 360) * TAU;
  mat3 m;
  m.a = cos(angle_tau);
  m.b = sin(angle_tau) * -1;
  m.d = sin(angle_tau);
  m.e = cos(angle_tau);
  m.i = 1.0;
  return m;
}

mat3 scale2(const vec2& s) {
  mat3 m;
  m.a = s.x;
  m.e = s.y;
  m.i = 1.0;
  return m;
}

mat3 translate2(const vec2& t) {
  mat3 m;
  m.a = 1.0;
  m.e = 1.0;
  m.i = 1.0;
  m.c = t.x;
  m.f = t.y;
  return m;
}

mat3 invert_y2() {
  mat3 m;
  m.a = 1.0;
  m.e = -1.0;
  m.i = 1.0;
  return m;
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

} // namespace clip


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

namespace fviz {

struct Point {
  Point();
  Point(double x, double y);
  double x;
  double y;
};

using vec2 = Point;

struct Rectangle {
  Rectangle();
  Rectangle(double x, double y, double w, double h);
  double x;
  double y;
  double w;
  double h;
};

double vec2_magnitude(const vec2& v);
vec2 vec2_normalize(const vec2& v);
vec2 vec2_add(const vec2& a, const vec2& b);
vec2 vec2_sub(const vec2& a, const vec2& b);
vec2 vec2_mul(const vec2& v, double s);

std::ostream& operator <<(std::ostream& os, const Point& c);
std::ostream& operator <<(std::ostream& os, const Rectangle& c);

} // namespace fviz


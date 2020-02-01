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
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include "vmath.h"

namespace clip {

using Point = vec2;

struct Rectangle {
  Rectangle();
  Rectangle(double x, double y, double w, double h);
  double x;
  double y;
  double w;
  double h;
};

std::ostream& operator <<(std::ostream& os, const Rectangle& c);

struct PolyLine2 {
  std::vector<vec2> vertices;
};

struct PolyLine3 {
  std::vector<vec3> vertices;
};

/**
 * - The poly lines are assumed to form closed 'rings'.
 */
struct Poly2 {
  PolyLine2 boundary;
  std::vector<PolyLine2> holes;
};

/**
 * - The poly lines are assumed to form closed 'rings'.
 */
struct Poly3 {
  PolyLine3 boundary;
  std::vector<PolyLine3> holes;
};

PolyLine2 polyline3_to_polyline2(const PolyLine3& p);

Poly2 poly3_to_poly2(const Poly3& p);

} // namespace clip


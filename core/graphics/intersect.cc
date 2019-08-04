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
#include "intersect.h"

namespace fviz {

/**
 * Check if two lines intersect and if they do, return the intersection
 * point
 */
bool intersect_line_line(
    vec2 o0,
    vec2 v0,
    vec2 o1,
    vec2 v1,
    vec2* x /* = nullptr */) {
  // ensure the direction vectors are normalized
  v0 = vec2_normalize(v0);
  v1 = vec2_normalize(v1);

  // consider lines as parallel if the angle between them is smaller than some
  // threshold
  const double a_epsilon = 0.001;
  double a = acos(vec2_dot(v0, v1));
  double a_norm = fmod(M_PI - a, M_PI);

  if (a_norm < a_epsilon) {
    return false;
  }

  return true;
}

} // namespace fviz


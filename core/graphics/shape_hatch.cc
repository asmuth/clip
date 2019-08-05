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
#include "shape_hatch.h"
#include "intersect.h"

namespace fviz {

Path shape_hatch(
    const Rectangle& clip,
    vec2 origin,
    double angle_deg,
    double offset,
    double stride,
    double width) {
  auto direction = vec2_from_deg(angle_deg);
  auto ortho = vec2_from_deg(angle_deg + 90);

  Polygon2 clip_poly;
  clip_poly.vertices = {
    {clip.x,          clip.y + clip.h},
    {clip.x + clip.w, clip.y + clip.h},
    {clip.x + clip.w, clip.y},
    {clip.x,          clip.y},
  };

  Path p;
  for (size_t i = 0; i < 100; ++i) {
    // symmetry around the origin
    for (double s = (i == 0 ? 0 : -1); s <= 1; s += 2) {

      // offset lines perpendicularly
      auto o = vec2_add(origin, vec2_mul(ortho, i * s * stride + offset));

      // find the intersections with the four borders of the clipping rectangle
      std::vector<Point> intersections;

      intersect_poly_line(
          clip_poly,
          o,
          direction,
          &intersections);

      // stroke line between edge points
      if (intersections.size() >= 2) {
        p.moveTo(intersections[0].x, clip.h - intersections[0].y);
        p.lineTo(intersections[1].x, clip.h - intersections[1].y);
      }
    }
  }

  return p;
}

} // namespace fviz


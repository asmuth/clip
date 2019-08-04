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

  Path p;
  for (size_t i = 0; i < 100; ++i) {
    // symmetry around the origin
    for (double s = (i == 0 ? 0 : -1); s <= 1; s += 2) {

      // offset lines perpendicularly
      auto o = vec2_add(origin, vec2_mul(ortho, i * s * stride + offset));

      // find the intersections with the four borders of the clipping rectangle
      std::vector<Point> intersections;

      // top border
      {
        Point p;
        auto p_found = intersect_line_lineseg(
            o,
            direction,
            {clip.x         , clip.y},
            {clip.x + clip.w, clip.y},
            &p);

        if (p_found) {
          intersections.push_back(p);
        }
      }

      // bottom border
      {
        Point p;
        auto p_found = intersect_line_lineseg(
            o,
            direction,
            {clip.x         , clip.y + clip.h},
            {clip.x + clip.w, clip.y + clip.h},
            &p);

        if (p_found) {
          intersections.push_back(p);
        }
      }

      // right border
      {
        Point p;
        auto p_found = intersect_line_lineseg(
            o,
            direction,
            {clip.x + clip.w, clip.y},
            {clip.x + clip.w, clip.y + clip.h},
            &p);

        if (p_found) {
          intersections.push_back(p);
        }
      }

      // left border
      {
        Point p;
        auto p_found = intersect_line_lineseg(
            o,
            direction,
            {clip.x, clip.y},
            {clip.x, clip.y + clip.h},
            &p);

        if (p_found) {
          intersections.push_back(p);
        }
      }

      // stroke line between edge points
      if (intersections.size() >= 2) {
        auto line_max = vec2_mul(
            vec2_normalize(vec2_sub(intersections[0], intersections[1])),
            vec2_magnitude({clip.w, clip.h}));

        // double the line length to make sure there are no gaps
        intersections[0] = vec2_add(intersections[0], line_max);
        intersections[1] = vec2_sub(intersections[1], line_max);

        p.moveTo(intersections[0].x, clip.h - intersections[0].y);
        p.lineTo(intersections[1].x, clip.h - intersections[1].y);
      }
    }
  }

  return p;
}

} // namespace fviz


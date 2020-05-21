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
#include "shape_hatch.h"
#include "intersect.h"

namespace clip {

Path shape_hatch(
    const Polygon2& clip,
    double angle_deg,
    double offset,
    double stride,
    double width) {
  auto origin = mean(clip.vertices.data(), clip.vertices.size());
  auto direction = from_deg(angle_deg);
  auto ortho = from_deg(angle_deg + 90);

  // return an empty path for pathological inputs
  if (stride == 0) {
    return {};
  }

  Path p;
  for (size_t i = 0; ; ++i) {
    bool edge_reached = true;

    // symmetry around the origin
    for (double s = (i == 0 ? 0 : -1); s <= 1; s += 2) {
      std::vector<Point> vertices;

      // find the intersections of two lines with the clipping rectangle
      //   - a "top" line that is offset by half the width in one direction
      //   - a "bottom" line that is offset by half the width in the other direction
      intersect_poly_line(
          clip,
          add(origin, mul(ortho, i * s * stride + offset + width * 0.5)),
          direction,
          &vertices);

      intersect_poly_line(
          clip,
          add(origin, mul(ortho, i * s * stride + offset + width * -0.5)),
          direction,
          &vertices);

      // TODO: find all vertices of the bounding polygon the lie between the
      // two lines and add them to the vertex list

      if (vertices.size() >= 3) {
        // we still had something to draw, so we're not ready yet
        edge_reached = false;
      } else {
        // if we end up with less than three vertices we are outside of the
        // clipping polygon
        continue;
      }

      // build a simple polygon from the resulting points by connecting them
      // in clockwise order
      sort_cw(vertices.data(), vertices.size());

      for (size_t i = 0; i < vertices.size(); ++i) {
        if (i == 0) {
          p.moveTo(vertices[i].x, vertices[i].y);
        } else {
          p.lineTo(vertices[i].x, vertices[i].y);
        }
      }

    }

    if (edge_reached) {
      break;
    }
  }

  return p;
}

} // namespace clip


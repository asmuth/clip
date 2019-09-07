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
#include <vector>
#include "geometry.h"
#include "polygon.h"

namespace clip {

enum class PathCommand {
  MOVE_TO,
  LINE_TO,
  QUADRATIC_CURVE_TO,
  CUBIC_CURVE_TO,
  CLOSE
};

const size_t kPathMaxCoefficients = 6;

struct PathData {
  PathCommand command;
  double coefficients[kPathMaxCoefficients];
  double operator[](size_t idx) const;
};

struct Path {
public:

  Path();
  Path(const PathData* data, size_t size);

  void moveTo(double x, double y);
  void moveTo(const Point& p);
  void lineTo(double x, double y);
  void lineTo(const Point& p);
  void quadraticCurveTo(double cx, double cy, double x, double y);
  void cubicCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y);
  void closePath();

  const PathData& operator[](size_t idx) const;
  PathData& operator[](size_t idx);

  const PathData* data() const;
  PathData* data();
  const PathData* begin() const;
  PathData* begin();
  const PathData* end() const;
  PathData* end();
  size_t size() const;
  bool empty() const;

protected:
  std::vector<PathData> data_;
};

/**
 * Convert a simple path that consists only of straight line segments and is free
 * of self-intersections to a polygon
 */
Polygon2 path_to_polygon_simple(const Path& path);

/**
 * Convert a polygon to a path
 */
Path path_from_polygon(const Polygon2& poly);

/**
 * Transform a path
 */
Path path_transform(const Path& p, const mat3& t);

/**
 * Add a line to the path.
 */
void path_add_line(Path* path, vec2 from, vec2 to);

/**
 * Add a rectangle to the path. Origin is the center of the rectangle.
 */
void path_add_rectangle(Path* path, vec2 origin, vec2 size);

/**
 * Add a circle to a path. Origin is the center of the circle.
 */
void path_add_circle(Path* path, vec2 origin, double radius);

} // namespace clip


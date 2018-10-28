/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

namespace signaltk {

enum class PathCommand {
  MOVE_TO,
  LINE_TO,
  QUADRATIC_CURVE_TO,
  CUBIC_CURVE_TO,
  ARC_TO,
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
  void lineTo(double x, double y);
  void quadraticCurveTo(double cx, double cy, double x, double y);
  void cubicCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y);
  void arcTo(double cx, double cy, double r, double a1, double a2);
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

} // namespace signaltk


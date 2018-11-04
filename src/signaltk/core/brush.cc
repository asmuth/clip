/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "brush.h"

namespace signaltk {

void strokePath(
    Layer* layer,
    const PathData* point_data,
    size_t point_count,
    const StrokeStyle& style) {
  layer->rasterizer.rasterizePath(point_data, point_count);
}

void strokeLine(
    Layer* layer,
    double x1,
    double y1,
    double x2,
    double y2,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(10, 10);
  p.lineTo(10, 100);
  p.lineTo(100, 100);
  p.lineTo(100, 10);
  p.closePath();
  strokePath(layer, p.data(), p.size(), style);
}

/* rasterize path using Maxim Shemanarev's libagg */

} // namespace signaltk


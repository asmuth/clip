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
#include "colour.h"
#include "layer.h"
#include "path.h"

namespace signaltk {

enum class StrokeLineJoin { MITER, ROUND, BEVEL };
enum class StrokeLineCap { BUTT, SQUARE, ROUND};

struct StrokeStyle {
  StrokeStyle() :
    line_width(1.0),
    line_join(StrokeLineJoin::MITER),
    line_cap(StrokeLineCap::SQUARE),
    colour(Colour::fromRGB(0, 0, 0)) {}

  float line_width;
  StrokeLineJoin line_join;
  StrokeLineCap line_cap;
  Colour colour;
};

void strokePath(
    Layer* layer,
    const PathData* path_data,
    size_t path_data_count,
    const StrokeStyle& style);

void strokeLine(
    Layer* layer,
    double x1,
    double y1,
    double x2,
    double y2,
    const StrokeStyle& style);

} // namespace signaltk


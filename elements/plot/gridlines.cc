/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include "graphics/layer.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "gridlines.h"
#include "domain.h"

namespace signaltk {
namespace chart {

GridDefinition::GridDefinition(
    kPlacement placement) :
    placement_(placement) {}

GridDefinition::kPlacement GridDefinition::placement() const {
  return placement_;
}

void GridDefinition::addTick(double tick_position) {
  ticks_.push_back(tick_position);
}

const std::vector<double> GridDefinition::ticks() const {
  return ticks_;
}

void renderGrid(
    const GridDefinition& grid,
    const Viewport& viewport,
    Layer* target) {
  StrokeStyle style;

  switch (grid.placement()) {

    case GridDefinition::GRID_HORIZONTAL:
      for (const auto& tick : grid.ticks()) {
        auto line_y = viewport.paddingTop() +
            viewport.innerHeight() * (1.0 - tick);

        strokeLine(
            target,
            viewport.paddingLeft(),
            line_y,
            viewport.paddingLeft() + viewport.innerWidth(),
            line_y,
            style);
      }
      break;

    case GridDefinition::GRID_VERTICAL:
      for (const auto& tick : grid.ticks()) {
        auto line_x = viewport.paddingLeft() + viewport.innerWidth() * tick;

        strokeLine(
            target,
            line_x,
            viewport.paddingTop(),
            line_x,
            viewport.paddingTop() + viewport.innerHeight(),
            style);
      }
      break;

  }
}

}
}

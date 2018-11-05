/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include <graphics/layer.h>
#include <graphics/viewport.h>

namespace signaltk {
namespace chart {

class GridDefinition {
public:

  enum kPlacement {
    GRID_HORIZONTAL = 0,
    GRID_VERTICAL = 1
  };

  /**
   * Create a new grid definition
   */
  GridDefinition(kPlacement placement);

  /**
   * Add a "tick" to this axis
   *
   * @param tick_position the position of the tick [0.0-1.0]
   */
  void addTick(double tick_position);

  kPlacement placement() const;

  const std::vector<double> ticks() const;

protected:
  kPlacement placement_;
  std::vector<double> ticks_;
};

/**
 * Render the grid
 */
void renderGrid(
    const GridDefinition& grid_config,
    const Viewport& viewport,
    Layer* target);

}
}


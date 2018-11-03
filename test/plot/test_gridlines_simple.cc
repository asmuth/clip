/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "signaltk/plot/gridlines.h"
#include "signaltk/util/fileutil.h"
#include "signaltk/util/stringutil.h"

using namespace signaltk;
using namespace signaltk::chart;

int main(int argc, char** argv) {
  Layer target(1000, 600);
  target.clear(Colour{1, 1, 1, 1});
  {
    GridDefinition grid(GridDefinition::GRID_HORIZONTAL);
    grid.addTick(0.0);
    grid.addTick(0.2);
    grid.addTick(0.4);
    grid.addTick(0.6);
    grid.addTick(0.8);
    grid.addTick(1.0);
    renderGrid(grid, Viewport{target.width, target.height}, &target);
  }

  {
    GridDefinition grid(GridDefinition::GRID_VERTICAL);
    grid.addTick(0.0);
    grid.addTick(0.2);
    grid.addTick(0.4);
    grid.addTick(0.6);
    grid.addTick(0.8);
    grid.addTick(1.0);
    renderGrid(grid, Viewport{target.width, target.height}, &target);
  }

  target.writePNG(std::string(argv[0]) + ".png");
}


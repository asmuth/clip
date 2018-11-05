/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <graphics/layer.h>
#include <components/plot/axes.h>
#include <components/plot/gridlines.h>
#include <utils/flagparser.h>
#include "plot_api.h"

namespace signaltk {

int cmd_plot_axes(Context* ctx, const char** args, int arg_count) {
  FlagParser flag_parser;

  std::string flag_in;
  flag_parser.defineString("in", true, &flag_in);

  std::string flag_out;
  flag_parser.defineString("out", true, &flag_out);

  bool flag_left;
  flag_parser.defineSwitch("left", &flag_left);

  auto rc = flag_parser.parseArgv(arg_count, args);
  if (!rc.isSuccess()) {
    std::cerr << "ERROR: " << rc.getMessage() << std::endl;
    return -1;
  }

  Layer target;
  if (!target.loadPNG(flag_in.c_str())) {
    return -1; // FIXME error
  }

  target.writePNG(flag_out.c_str());

  return 0;
}

int cmd_plot_gridlines(Context* ctx, const char** args, int arg_count) {
  FlagParser flag_parser;

  std::string flag_in;
  flag_parser.defineString("in", true, &flag_in);

  std::string flag_out;
  flag_parser.defineString("out", true, &flag_out);

  auto rc = flag_parser.parseArgv(arg_count, args);
  if (!rc.isSuccess()) {
    std::cerr << "ERROR: " << rc.getMessage() << std::endl;
    return -1;
  }

  Layer target;
  if (!target.loadPNG(flag_in.c_str())) {
    return -1; // FIXME error
  }

  {
    chart::GridDefinition grid(chart::GridDefinition::GRID_HORIZONTAL);
    grid.addTick(0.0);
    grid.addTick(0.2);
    grid.addTick(0.4);
    grid.addTick(0.6);
    grid.addTick(0.8);
    grid.addTick(1.0);
    chart::renderGrid(grid, Viewport{target.width, target.height}, &target);
  }

  {
    chart::GridDefinition grid(chart::GridDefinition::GRID_VERTICAL);
    grid.addTick(0.0);
    grid.addTick(0.2);
    grid.addTick(0.4);
    grid.addTick(0.6);
    grid.addTick(0.8);
    grid.addTick(1.0);
    chart::renderGrid(grid, Viewport{target.width, target.height}, &target);
  }

  target.writePNG(flag_out.c_str());

  return 0;
}

} // namespace signaltk


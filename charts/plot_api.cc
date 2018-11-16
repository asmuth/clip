/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>
#include <graphics/layer.h>
#include <elements/plot/axes.h>
#include <elements/plot/gridlines.h>
#include <utils/flagparser.h>
#include "plot_api.h"

namespace plotfx {

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

} // namespace plotfx


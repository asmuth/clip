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
#include "command.h"
#include "plot/axis.h"
#include "plot/grid.h"
#include "plot/lines.h"
#include "figure/legend.h"

namespace clip {

const CommandMap COMMANDS = {
  {"default", CommandFn(&context_set_default)},
  {"set-width", CommandFn(&context_configure)},
  {"set-height", CommandFn(&context_configure)},
  {"set-dpi", CommandFn(&context_configure)},
  {"layout/add-margins", CommandFn(&layout_add_margins)},
  {"plot/add-axes", CommandFn(&elements::plot::axis::axis_add_all)},
  {"plot/draw-axis", CommandFn(&elements::plot::axis::axis_draw)},
  {"plot/draw-grid", CommandFn(&elements::plot::grid::draw_grid)},
  {"plot/draw-lines", CommandFn(&elements::plot::lines::draw_lines)},
  {"figure/draw-legend", CommandFn(&elements::legend::legend_draw)},
};

} // namespace clip



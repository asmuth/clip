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
#include "plot/areas.h"
#include "plot/axis.h"
#include "plot/bars.h"
#include "plot/errorbars.h"
#include "plot/grid.h"
#include "plot/labels.h"
#include "plot/lines.h"
#include "plot/points.h"
#include "plot/rectangles.h"
#include "plot/vectors.h"
#include "figure/legend.h"

namespace clip {

const CommandMap COMMANDS = {
  {"default", CommandFn(&context_set_default)},
  {"set-width", CommandFn(&context_configure)},
  {"set-height", CommandFn(&context_configure)},
  {"set-dpi", CommandFn(&context_configure)},
  {"layout/add-margins", CommandFn(&layout_add_margins)},
  {"plot/add-axes", CommandFn(&elements::plot::axis::axis_add_all)},
  {"plot/draw-areas", CommandFn(&elements::plot::areas::areas_draw)},
  {"plot/draw-axis", CommandFn(&elements::plot::axis::axis_draw)},
  {"plot/draw-bars", CommandFn(&elements::plot::bars::bars_draw)},
  {"plot/draw-errorbars", CommandFn(&elements::plot::errorbars::errorbars_draw)},
  {"plot/draw-grid", CommandFn(&elements::plot::grid::draw_grid)},
  {"plot/draw-labels", CommandFn(&elements::plot::labels::labels_draw)},
  {"plot/draw-lines", CommandFn(&elements::plot::lines::draw_lines)},
  {"plot/draw-points", CommandFn(&elements::plot::points::points_draw)},
  {"plot/draw-rectangles", CommandFn(&elements::plot::rectangles::rectangles_draw)},
  {"plot/draw-vectors", CommandFn(&elements::plot::vectors::vectors_draw)},
  {"figure/draw-legend", CommandFn(&elements::legend::legend_draw)},
};

} // namespace clip



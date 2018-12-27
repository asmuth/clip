/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include "plot_gridlines.h"
#include "document.h"

using namespace std::placeholders;

namespace plotfx {
namespace plot {

static const double kDefaultLineWidthPT = 1;

ReturnCode grid_configure(
    const PropertyList& plist,
    const Document& doc,
    GridlineDefinition* grid) {
  Measure line_width;
  Color line_color = Color::fromRGB(.9, .9, .9); // TODO

  static const ParserDefinitions pdefs = {
    {"grid-stroke", bind(&configure_measure_rel, _1, doc.dpi, doc.font_size, &line_width)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  grid->line_width = measure_or(line_width, from_pt(kDefaultLineWidthPT, doc.dpi));
  grid->line_color = line_color;
  return OK;
}

ReturnCode grid_draw(
    const GridlineDefinition& grid,
    const Rectangle& bbox,
    Layer* layer) {
  StrokeStyle style;
  style.line_width = grid.line_width;
  style.color = grid.line_color;

  for (const auto& tick : grid.ticks_horiz) {
    auto line_y = bbox.y + bbox.h * (1.0 - tick);

    strokeLine(
        layer,
        Point(bbox.x, line_y),
        Point(bbox.x + bbox.w, line_y),
        style);
  }

  for (const auto& tick : grid.ticks_vert) {
    auto line_x = bbox.x + bbox.w * tick;

    strokeLine(
        layer,
        Point(line_x, bbox.y),
        Point(line_x, bbox.y + bbox.h),
        style);
  }

  return OK;
}

} // namespace plot
} // namespace plotfx


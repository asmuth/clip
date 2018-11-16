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
#include <stdlib.h>
#include "graphics/layer.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "gridlines.h"

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

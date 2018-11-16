/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
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


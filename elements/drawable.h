/**
 * This file is part of the "plotfx" project
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
#ifndef _libstx_DRAWABLE_H
#define _libstx_DRAWABLE_H
#include <tuple>
#include <functional>
#include "axisdefinition.h"
#include "griddefinition.h"
#include "legenddefinition.h"
#include "series.h"
#include "../core/viewport.h"

namespace plotfx {
namespace chart {
class Layer;
class Canvas;

// FIXPAUL: rename to chart
class Drawable {
  friend class Canvas;
public:
  Drawable(Canvas* canvas);
  virtual ~Drawable();

  /**
   * Set the title for this chart
   */
  void setTitle(const std::string& title);

  /**
   * Set the subtitle for this chart
   */
  void setSubtitle(const std::string& subtitle);

  /**
   * Add an axis to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param position the position/placement of the axis
   */
  virtual AxisDefinition* addAxis(AxisDefinition::kPosition position) = 0;

  /**
   * Add a grid to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param vertical render vertical grid lines?
   * @param horizontal render horizonyal grid lines?
   */
  virtual GridDefinition* addGrid(GridDefinition::kPlacement placement) = 0;

  /**
   * Add a legend to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * FIXPAUL params
   */
  LegendDefinition* addLegend(
      LegendDefinition::kVerticalPosition vert_pos,
      LegendDefinition::kHorizontalPosition horiz_pos,
      LegendDefinition::kPlacement placement,
      const std::string& title);

  /**
   * Get the {x,y,z} domain of this chart. May raise an exception if the chart
   * does not implement the requested domain.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param dimension the dimension
   */
  virtual AnyDomain* getDomain(AnyDomain::kDimension dimension) = 0;

protected:

  void addSeries(Series* series);
  virtual void render(Layer* target, Viewport* viewport) const = 0;

  Canvas* canvas_;
private:
  void updateLegend();
  std::vector<Series*> all_series_;
};

}
}
#endif

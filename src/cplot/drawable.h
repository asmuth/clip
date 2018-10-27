/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_DRAWABLE_H
#define _libstx_DRAWABLE_H
#include <tuple>
#include <functional>
#include "cplot/axisdefinition.h"
#include "cplot/griddefinition.h"
#include "cplot/legenddefinition.h"
#include "cplot/series.h"
#include "cplot/viewport.h"

namespace stx {
namespace chart {
class RenderTarget;
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
  virtual void render(RenderTarget* target, Viewport* viewport) const = 0;

  Canvas* canvas_;
private:
  void updateLegend();
  std::vector<Series*> all_series_;
};

}
}
#endif

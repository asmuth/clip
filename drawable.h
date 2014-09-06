/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_DRAWABLE_H
#define _FNORDMETRIC_DRAWABLE_H
#include <tuple>
#include <functional>
#include <fnordmetric/base/series.h>
#include <fnordmetric/ui/axisdefinition.h>
#include <fnordmetric/ui/viewport.h>

namespace fnordmetric {
namespace ui {
class RenderTarget;
class Canvas;

// FIXPAUL: rename to chart
class Drawable {
  friend class Canvas;
public:

  Drawable(Canvas* canvas) : canvas_(canvas) {}
  virtual ~Drawable() {}

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
   */
  void addGrid(bool horizontal, bool vertical) {
    RAISE(
        util::RuntimeException,
        "grid not implemented: horizontal=%s, vertical=%s",
        horizontal ? "true" : "false",
        vertical ? "true" : "false");
  }

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

  virtual void render(RenderTarget* target, Viewport* viewport) const = 0;

  Canvas* canvas_;
};

}
}
#endif

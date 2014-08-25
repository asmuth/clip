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
#include "../base/series.h"

namespace fnordmetric {
namespace ui {
class RenderTarget;

class Drawable {
  friend class Canvas;
public:

  Drawable() : color_index_(0) {}
  virtual ~Drawable() {}

  /**
   * Add an axis to the chart. This method should only be called after all
   * series have been added to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param position the position/placement of the axis
   */
   virtual AxisDefinition* addAxis(AxisDefinition::kPosition position) = 0;

protected:

  const std::string& seriesColor(Series* series) {
    if (series->getColor().size() == 0) {
      char buf[32];
      size_t size = snprintf(buf, sizeof(buf), "color%i", ++color_index_);
      series->setColor(std::string(buf, size));
    }

    return series->getColor();
  }

  virtual void render(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const = 0;

  int color_index_;
};

}
}
#endif

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
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

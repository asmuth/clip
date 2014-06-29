/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_BARCHART_H
#define _FNORDMETRIC_BARCHART_H
#include <stdlib.h>
#include <assert.h>
#include "../drawable.h"

namespace fnordmetric {

/**
 *
 * OPTIONS
 *
 *   height             = int
 *   width              = int
 *   orientation        = {horizontal,vertical}, default: horizontal
 *   stacked            = {on,off}, default: off
 *   show_labels        = {inside,outside,auto,off}, default: auto
 *   rotate_labels      = {on,off,auto}, default: auto
 *   label_color        = color
 *
 */
class BarChart : public Drawable {
public:
  enum kBarChartOrientation {
    O_VERTICAL,
    O_HORIZONTAL
  };

  BarChart();

  void draw(ChartRenderTarget* target) override;

protected:

  void drawHorizontalBars(ChartRenderTarget* target);

  kBarChartOrientation orientation_;
  bool show_bar_values_;
  int width_;
  int height_;
};

}
#endif

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "barchart.h"

namespace fnordmetric {

BarChart::BarChart() :
    orientation_(O_HORIZONTAL),
    show_labels_(true),
    width_(800),
    height_(400) {}

void BarChart::draw(ChartRenderTarget* target) {
  target->beginChart(420, 120);

  switch (orientation_) {
    case O_HORIZONTAL:
      drawHorizontalBars(target);
      break;
  }

  target->finishChart();
}

void drawHorizontalBars(ChartRenderTarget* target) {

}

}

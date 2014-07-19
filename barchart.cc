/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "barchart.h"
#include "domain.h"

/**
 * todo:
 *  - labels inside/outside
 */
namespace fnordmetric {
namespace ui {

BarChart::BarChart(
    Canvas* canvas,
    NumericalDomain* y_domain /* = nullptr */,
    kBarChartOrientation orientation /* = O_HORIZONTAL */) :
    orientation_(orientation),
    y_domain_(y_domain) {}

/*
void BarChart::draw(ChartRenderTarget* target) {
  //prepareData();

  switch (orientation_) {
    case O_VERTICAL:
      //drawVerticalBars(target);
      break;
    case O_HORIZONTAL:
      //drawHorizontalBars(target);
      break;
  }
}
*/



}
}

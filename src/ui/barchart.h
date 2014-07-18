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
#include "domain.h"

namespace fnordmetric {
class Domain;

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

  static const double kBarPadding = 0.2f; // FIXPAUL make configurable

  BarChart();

  void draw(ChartRenderTarget* target) override;

protected:
  struct BarData {
    query::SValue x;
    std::vector<std::pair<double, double>> ys;
  };

  void drawVerticalBars(ChartRenderTarget* target);
  void drawHorizontalBars(ChartRenderTarget* target);
  void prepareData();

  std::pair<double, double> scaleValue(
      const query::SValue* value, 
      const Domain* domain) const;

  kBarChartOrientation orientation_;
  bool stacked_;
  std::vector<BarData> data_;
  Domain y_domain;
};

}
#endif

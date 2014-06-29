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

namespace fnordmetric {

BarChart::BarChart() :
    orientation_(O_VERTICAL),
    width_(800),
    height_(400),
    padding_top_(0),
    padding_left_(0),
    padding_bottom_(0),
    padding_right_(0) {}

void BarChart::draw(ChartRenderTarget* target) {
  Domain y_domain(0, 50, false);

  target->beginChart(width_, height_);

  switch (orientation_) {
    case O_VERTICAL:
      drawVerticalBars(target, &y_domain);
      break;
    case O_HORIZONTAL:
      drawHorizontalBars(target);
      break;
  }

  target->finishChart();
}

void BarChart::drawVerticalBars(ChartRenderTarget* target, Domain* y_domain) {
  /* how much space do we have to draw all the bars? */
  int width = width_ - (padding_left_ + padding_right_);
  int height = height_ - (padding_top_ + padding_bottom_);

  /* find the number of bars we need to draw */
  int num_bars = 0;
  for (const auto& series : getSeries()) {
    auto series_len = series->getData().size();
    if (series_len > num_bars) {
      num_bars = series_len;
    }
  }

  /* calculate bar width and padding */
  double bar_padding_ratio = 0.1f; // FIXPAUL make configurable
  auto bar_width = (width / num_bars) * (1.0f - bar_padding_ratio);
  auto bar_padding = (width / num_bars) * (bar_padding_ratio * 0.5f);

  /* draw the bars */
  auto draw_x = padding_left_;
  auto draw_width = bar_width;
  for (int i = 0; i < num_bars; ++i) {
    auto y_min = y_domain->scale(i);
    auto y_max = y_domain->scale(3 * i + 1);
    auto draw_y = padding_top_ + ((1.0f - y_max) * height);
    auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * height;
    draw_x += bar_padding;
    target->drawRect(draw_x, draw_y, draw_width, draw_height);
    draw_x += bar_width + bar_padding;
  }
}

void BarChart::drawHorizontalBars(ChartRenderTarget* target) {

}

}

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
  prepareData();

  /* how much space do we have to draw all the bars? */
  inner_width_ = width_ - (padding_left_ + padding_right_);
  inner_height_ = height_ - (padding_top_ + padding_bottom_);

  target->beginChart(width_, height_);

  /* draw the bars */
  switch (orientation_) {
    case O_VERTICAL:
      drawVerticalBars(target);
      break;
    case O_HORIZONTAL:
      drawHorizontalBars(target);
      break;
  }

  target->finishChart();
}

void BarChart::prepareData() {
  /* setup our value domain */
  Domain y_domain(0, 50, false);

  for (const auto& series : getSeries()) {
    // FIXPAUL this could be O(N) but is O(N*2)
    for (const auto& datum : series->getData()) {
      const auto& x_val = datum[0];
      const auto& y_val = datum[1];

      BarData* bar_data = nullptr;
      for (auto& candidate : data_) {
        if (candidate.x == x_val) {
          bar_data = &candidate;
        }
      }

      if (bar_data == nullptr) {
        data_.emplace_back();
        bar_data = &data_.back();
        bar_data->x = x_val;
      }

      bar_data->ys.push_back(scaleValue(&y_val, &y_domain));
    }
  }
}

std::pair<double, double> BarChart::scaleValue(
    const query::SValue* value,
    const Domain* domain) const {
  switch (value->getType()) {
    case query::SValue::T_INTEGER:
      return std::pair<double, double>(
          0.0f,
          domain->scale(value->getInteger()));
    case query::SValue::T_FLOAT:
      return std::pair<double, double>(
          0.0f,
          domain->scale(value->getFloat()));
    default:
      assert(0); // FIXPAUL
  }
}

void BarChart::drawVerticalBars(ChartRenderTarget* target) {
  /* calculate bar width and padding */
  double bar_padding_ratio = 0.1f; // FIXPAUL make configurable
  auto bar_width = (inner_width_ / data_.size()) * (1.0f - bar_padding_ratio);
  auto bar_padding = (inner_width_ / data_.size()) * (bar_padding_ratio * 0.5f);

  /* draw the bars */
  auto draw_x = padding_left_;
  auto draw_width = bar_width;
  for (const auto& bar : data_) {
    for (const auto& y_val : bar.ys) {
      auto y_min = y_val.first;
      auto y_max = y_val.second;
      auto draw_y = padding_top_ + ((1.0f - y_max) * inner_height_);
      auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * inner_height_;
      draw_x += bar_padding;
      target->drawRect(draw_x, draw_y, draw_width, draw_height);
      draw_x += bar_width + bar_padding;
    }
  }
}

void BarChart::drawHorizontalBars(ChartRenderTarget* target) {

}

}

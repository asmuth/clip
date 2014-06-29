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
    orientation_(O_HORIZONTAL) {}

void BarChart::draw(ChartRenderTarget* target) {
  prepareData();
  target->beginChart(width_, height_);
  Drawable::draw(target);

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
  y_domain = Domain(0, 50, false);

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
  if (show_axis_[LEFT]) {
    drawLeftAxis(target, &y_domain);
  }

  if (show_axis_[RIGHT]) {
    drawRightAxis(target, &y_domain);
  }

  if (show_axis_[BOTTOM]) {
    drawBottomAxis(target, &y_domain);
  }

  if (show_axis_[TOP]) {
    drawTopAxis(target, &y_domain);
  }

  /* calculate bar width and padding */
  auto bar_width = (inner_width_ / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (inner_width_ / data_.size()) * (kBarPadding * 0.5f);
  bar_width -= bar_padding / data_.size() * 2;

  /* draw the bars */
  auto draw_x = padding_left_ + bar_padding;
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
  if (show_axis_[LEFT]) {
    drawLeftAxis(target, &y_domain);
  }

  if (show_axis_[RIGHT]) {
    drawRightAxis(target, &y_domain);
  }

  if (show_axis_[BOTTOM]) {
    drawBottomAxis(target, &y_domain);
  }

  if (show_axis_[TOP]) {
    drawTopAxis(target, &y_domain);
  }

  /* calculate bar width and padding */
  auto bar_height = (inner_height_ / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (inner_height_ / data_.size()) * (kBarPadding * 0.5f);
  bar_height -= bar_padding / data_.size() * 2;

  /* draw the bars */
  auto draw_y = padding_top_ + bar_padding;
  auto draw_height = bar_height;
  for (const auto& bar : data_) {
    for (const auto& y_val : bar.ys) {
      auto y_min = y_val.first;
      auto y_max = y_val.second;
      auto draw_x = padding_left_ + y_min * inner_width_;
      auto draw_width = (y_max - y_min) * inner_width_;
      draw_y += bar_padding;
      target->drawRect(draw_x, draw_y, draw_width, draw_height);
      draw_y += bar_height + bar_padding;
    }
  }
}

}

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
 *  - stacked
 *  - labels inside/outside
 *  - calculate domain
 */
namespace fnordmetric {

BarChart::BarChart() :
    orientation_(O_HORIZONTAL),
    stacked_(false) {}

void BarChart::draw(ChartRenderTarget* target) {
  prepareData();

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
  y_domain = Domain(0, 40, false);

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
          domain->scale(0.0f),
          domain->scale(value->getInteger()));
    case query::SValue::T_FLOAT:
      return std::pair<double, double>(
          domain->scale(0.0f),
          domain->scale(value->getFloat()));
    default:
      assert(0); // FIXPAUL
  }
}

void BarChart::drawVerticalBars(ChartRenderTarget* target) {
  target->beginChart(width_, height_, "chart bar vertical");
  Drawable::draw(target);

  /* calculate bar width and padding */
  auto bar_width = (inner_width_ / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (inner_width_ / data_.size()) * (kBarPadding * 0.5f);
  bar_width -= bar_padding / data_.size() * 2;

  /* draw the bars */
  std::vector<double> x_ticks = {0.0f};
  std::vector<std::pair<double, std::string>> x_labels;
  auto draw_x = padding_left_ + bar_padding;
  auto draw_width = bar_width;
  for (const auto& bar : data_) {
    draw_x += bar_padding;

    /* single series */
    if (getSeries().size() == 1) {
      auto& y_val = bar.ys[0];
      auto y_min = y_val.first;
      auto y_max = y_val.second;
      auto draw_y = padding_top_ + ((1.0f - y_max) * inner_height_);
      auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * inner_height_;
      target->drawRect(draw_x, draw_y, draw_width, draw_height, colorName(0));
    }

    /* multi series stacked */
    else if (stacked_) {
    }

    /* multi series unstacked */
    else {
      auto num_series = getSeries().size();
      auto draw_x_multi = draw_x;
      auto draw_width_multi = draw_width / num_series;
      for (int i = 0; i < bar.ys.size(); i++) {
        auto& y_val = bar.ys[i];
        auto y_min = y_val.first;
        auto y_max = y_val.second;
        auto draw_y = padding_top_ + ((1.0f - y_max) * inner_height_);
        auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * inner_height_;
        target->drawRect(
            draw_x_multi,
            draw_y,
            draw_width_multi * (1.0f - kBarPadding * 0.5f),
            draw_height,
            colorName(i));
        draw_x_multi += (draw_width_multi * (1.0f + kBarPadding * 0.5f));
      }
    }

    x_labels.push_back(std::make_pair((
        draw_x - padding_left_ + bar_width * 0.5f) / inner_width_,
        format::svalueToHuman(bar.x)));
    draw_x += bar_width + bar_padding;
    x_ticks.push_back((draw_x - padding_left_) / inner_width_);
  }
  x_ticks.back() = 1.0f;

  if (show_axis_[LEFT]) {
    drawLeftAxis(target, &y_domain);
  }

  if (show_axis_[RIGHT]) {
    drawRightAxis(target, &y_domain);
  }

  if (show_axis_[BOTTOM]) {
    drawBottomAxis(target, x_ticks, x_labels);
  }

  if (show_axis_[TOP]) {
    drawTopAxis(target, &y_domain);
  }
}

void BarChart::drawHorizontalBars(ChartRenderTarget* target) {
  target->beginChart(width_, height_, "chart bar horizontal");
  Drawable::draw(target);

  /* calculate bar width and padding */
  auto bar_height = (inner_height_ / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (inner_height_ / data_.size()) * (kBarPadding * 0.5f);
  bar_height -= bar_padding / data_.size() * 2;

  /* draw the bars */
  std::vector<double> y_ticks = {0.0f};
  std::vector<std::pair<double, std::string>> y_labels;
  auto draw_y = padding_top_ + bar_padding;
  auto draw_height = bar_height;
  for (const auto& bar : data_) {
    draw_y += bar_padding;

    /* single series */
    if (getSeries().size() == 1) {
      auto& y_val = bar.ys[0];
      auto y_min = y_val.first;
      auto y_max = y_val.second;
      auto draw_x = padding_left_ + y_min * inner_width_;
      auto draw_width = (y_max - y_min) * inner_width_;
      target->drawRect(draw_x, draw_y, draw_width, draw_height, colorName(0));
    }

    /* multi series stacked */
    else if (stacked_) {
    }

    /* multi series unstacked */
    else {
      auto num_series = getSeries().size();
      auto draw_y_multi = draw_y;
      auto draw_height_multi = draw_height / num_series;
      for (int i = 0; i < bar.ys.size(); i++) {
        auto& y_val = bar.ys[i];
        auto y_min = y_val.first;
        auto y_max = y_val.second;
        auto draw_x = padding_left_ + y_min * inner_width_;
        auto draw_width = (y_max - y_min) * inner_width_;
        target->drawRect(
            draw_x,
            draw_y_multi,
            draw_width,
            draw_height_multi * (1.0f - kBarPadding * 0.5f),
            colorName(i));
        draw_y_multi += (draw_height_multi * (1.0f + kBarPadding * 0.5f));
      }
    }

    y_labels.push_back(std::make_pair((
        draw_y - padding_top_ + bar_height * 0.5f) / inner_height_,
        format::svalueToHuman(bar.x)));
    draw_y += bar_height + bar_padding;
    y_ticks.push_back((draw_y - padding_top_) / inner_height_);
  }
  y_ticks.back() = 1.0f;

  if (show_axis_[LEFT]) {
    drawLeftAxis(target, y_ticks, y_labels);
  }

  if (show_axis_[RIGHT]) {
    drawRightAxis(target, y_ticks, y_labels);
  }

  if (show_axis_[BOTTOM]) {
    drawBottomAxis(target, &y_domain);
  }

  if (show_axis_[TOP]) {
    drawTopAxis(target, &y_domain);
  }
}

}

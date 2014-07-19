/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "barchart.h"
#include "canvas.h"
#include "domain.h"
#include "rendertarget.h"

/**
 * todo:
 *  - labels inside/outside
 */
namespace fnordmetric {
namespace ui {

BarChart::BarChart(
    Canvas* canvas,
    kBarChartOrientation orientation /* = O_HORIZONTAL */,
    bool stacked /* = false */,
    NumericalDomain* y_domain /* = nullptr */) :
    canvas_(canvas),
    orientation_(orientation),
    stacked_(stacked),
    y_domain_(y_domain),
    num_series_(0) {}


void BarChart::addSeries(Series2D<std::string, double>* series) {
  for (const auto& point : series->getData()) {
    const auto& x_val = std::get<0>(point);
    const auto& y_val = std::get<1>(point);

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

    bar_data->ys.emplace_back(0, y_val);   //scaleValue(&y_val, &y_domain));
  }

  num_series_++;
}

AxisDefinition* BarChart::addAxis(AxisDefinition::kPosition position) {
  switch (position) {

    case AxisDefinition::TOP:
      switch (orientation_) {
        case O_VERTICAL:
          return newLabelAxis(position);
          break;
        case O_HORIZONTAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
      }
      break;

    case AxisDefinition::RIGHT:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
        case O_HORIZONTAL:
          return newLabelAxis(position);
          break;
      }
      break;

    case AxisDefinition::BOTTOM:
      switch (orientation_) {
        case O_VERTICAL:
          return newLabelAxis(position);
          break;
        case O_HORIZONTAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
      }
      break;

    case AxisDefinition::LEFT:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
        case O_HORIZONTAL:
          return newLabelAxis(position);
          break;
      }
      break;

  }
}

NumericalDomain* BarChart::getValueDomain() const {
  if (y_domain_ != nullptr) {
    return y_domain_;
  }

  if (y_domain_auto_.get() == nullptr) {
    y_domain_auto_.reset(newValueDomain());
  }

  return y_domain_auto_.get();
}

NumericalDomain* BarChart::newValueDomain() const {
  /* calculate our domain*/
  if (stacked_) {
    double y_domain_max = 0.0f;

    for (const auto& group : data_) {
      int sum = 0;
      for (const auto& y : group.ys) {
        sum += y.second;
      }

      if (sum > 0 && sum > y_domain_max) {
        y_domain_max = sum;
      }
    }

    if (y_domain_max > 0) {
      y_domain_max *= 1.1;
    }

    return new NumericalDomain(0, y_domain_max, false);
  } else {
    double y_domain_min = 0.0f;
    double y_domain_max = 0.0f;

    for (const auto& group : data_) {
      for (const auto& y : group.ys) {
        if (y.first < y_domain_min) {
          y_domain_min = y.first;
        }
        if (y.first > y_domain_max) {
          y_domain_max = y.first;
        }
        if (y.second < y_domain_min) {
          y_domain_min = y.second;
        }
        if (y.second > y_domain_max) {
          y_domain_max = y.second;
        }
      }
    }

    if (y_domain_max > 0) {
      y_domain_max *= 1.1;
    }

    return new NumericalDomain(y_domain_min, y_domain_max, false);
  }
}

AxisDefinition* BarChart::newLabelAxis(AxisDefinition::kPosition position)
    const {
  auto axis = canvas_->addAxis(position);
  axis->addTick(0.0f);

  auto bar_width = (1.0 / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (1.0 / data_.size()) * (kBarPadding * 0.5f);
  bar_width -= bar_padding / data_.size() * 2;

  auto draw_x = bar_padding;
  for (int i = 0; i < data_.size(); ++i) {
    draw_x += bar_padding;

    switch (orientation_) {
      case O_VERTICAL:
        axis->addLabel(draw_x + bar_width * 0.5f, data_[i].x);
        break;
      case O_HORIZONTAL:
        axis->addLabel(
            draw_x + bar_width * 0.5f,
            data_[data_.size() - 1 - i].x);
        break;
    }

    draw_x += bar_width + bar_padding;
    if (i < data_.size() - 1) {
      axis->addTick(draw_x);
    }
  }

  axis->addTick(1.0f);
  return axis;
}

void BarChart::render(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  switch (orientation_) {
    case O_VERTICAL:
      renderVerticalBars(target, width, height, padding);
      break;
    case O_HORIZONTAL:
      renderHorizontalBars(target, width, height, padding);
      break;
  }
}

void BarChart::renderVerticalBars(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  /* calculate bar width and padding */
  auto padding_top = std::get<0>(*padding);
  auto padding_right = std::get<1>(*padding);
  auto padding_bottom = std::get<2>(*padding);
  auto padding_left = std::get<3>(*padding);
  auto inner_width = width - padding_right - padding_left;
  auto inner_height = height - padding_top - padding_bottom;
  auto bar_width = (inner_width / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (inner_width / data_.size()) * (kBarPadding * 0.5f);
  bar_width -= bar_padding / data_.size() * 2;

  /* draw the bars */
  std::vector<double> x_ticks = {0.0f};
  auto draw_x = padding_left + bar_padding;
  auto draw_width = bar_width;
  auto y_domain = getValueDomain();
  for (const auto& bar : data_) {
    draw_x += bar_padding;

    /* single series */
    if (num_series_ == 1) {
      auto y_min = y_domain->scale(bar.ys[0].first);
      auto y_max = y_domain->scale(bar.ys[0].second);
      auto draw_y = padding_top + ((1.0f - y_max) * inner_height);
      auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * inner_height;
      target->drawRect(draw_x, draw_y, draw_width, draw_height, "color0");
    }

    /* multi series stacked */
    else if (stacked_) {
      double y_min = 0.0f;
      double y_max = 0.0f;
      for (int i = 0; i < bar.ys.size(); i++) {
        auto& y_val = bar.ys[i];
        y_max += y_val.second - y_val.first;
        auto draw_y = padding_top + 
            ((1.0f - y_domain->scale(y_max)) * inner_height);
        auto draw_height = (1.0f - ((1.0f - y_domain->scale(y_max)) +
            y_domain->scale(y_min))) * inner_height;
        target->drawRect(draw_x, draw_y, draw_width, draw_height, "color0");
        y_min += y_val.second - y_val.first;
      }
    }

    /* multi series unstacked */
    else {
      auto draw_x_multi = draw_x;
      auto draw_width_multi = draw_width / (double) num_series_;
      for (int i = 0; i < bar.ys.size(); i++) {
      auto y_min = y_domain->scale(bar.ys[i].first);
      auto y_max = y_domain->scale(bar.ys[i].second);
        auto draw_y = padding_top + ((1.0f - y_max) * inner_height);
        auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * inner_height;
        target->drawRect(
            draw_x_multi,
            draw_y,
            draw_width_multi * (1.0f - kBarPadding * 0.5f),
            draw_height,
            "color0");
        draw_x_multi += (draw_width_multi * (1.0f + kBarPadding * 0.5f));
      }
    }

    draw_x += bar_width + bar_padding;
  }
}
void BarChart::renderHorizontalBars(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  /* calculate bar width and padding */
  auto padding_top = std::get<0>(*padding);
  auto padding_right = std::get<1>(*padding);
  auto padding_bottom = std::get<2>(*padding);
  auto padding_left = std::get<3>(*padding);
  auto inner_width = width - padding_right - padding_left;
  auto inner_height = height - padding_top - padding_bottom;
  auto bar_height = (inner_height / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (inner_height / data_.size()) * (kBarPadding * 0.5f);
  bar_height -= bar_padding / data_.size() * 2;

  /* draw the bars */
  std::vector<double> y_ticks = {0.0f};
  std::vector<std::pair<double, std::string>> y_labels;
  auto draw_y = padding_top + bar_padding;
  auto draw_height = bar_height;
  auto y_domain = getValueDomain();
  for (const auto& bar : data_) {
    draw_y += bar_padding;

    /* single series */
    if (num_series_ == 1) {
      auto& y_val = bar.ys[0];
      auto y_min = y_domain->scale(bar.ys[0].first);
      auto y_max = y_domain->scale(bar.ys[0].second);
      auto draw_x = padding_left + y_min * inner_width;
      auto draw_width = (y_max - y_min) * inner_width;
      target->drawRect(draw_x, draw_y, draw_width, draw_height, "color0");
    }

    /* multi series stacked */
    else if (stacked_) {
      double y_min = 0.0f;
      double y_max = 0.0f;
      for (int i = 0; i < bar.ys.size(); i++) {
        auto& y_val = bar.ys[i];
        y_max += y_val.second - y_val.first;
        auto draw_x = padding_left + y_domain->scale(y_min) * inner_width;
        auto draw_width = y_domain->scale(y_max - y_min) * inner_width;
        target->drawRect(draw_x, draw_y, draw_width, draw_height, "color0");
        y_min += y_val.second - y_val.first;
      }
    }

    /* multi series unstacked */
    else {
      auto draw_y_multi = draw_y;
      auto draw_height_multi = draw_height / num_series_;
      for (int i = 0; i < bar.ys.size(); i++) {
      auto y_min = y_domain->scale(bar.ys[i].first);
      auto y_max = y_domain->scale(bar.ys[i].second);
        auto draw_x = padding_left + y_min * inner_width;
        auto draw_width = (y_max - y_min) * inner_width;
        target->drawRect(
            draw_x,
            draw_y_multi,
            draw_width,
            draw_height_multi * (1.0f - kBarPadding * 0.5f),
            "color0");
        draw_y_multi += (draw_height_multi * (1.0f + kBarPadding * 0.5f));
      }
    }

    draw_y += bar_height + bar_padding;
  }
}


}
}

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_BARCHART_H
#define _FNORDMETRIC_BARCHART_H
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <fnordmetric/base/series.h>
#include <fnordmetric/ui/axisdefinition.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/domain.h>
#include <fnordmetric/ui/drawable.h>
#include <fnordmetric/ui/rendertarget.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace ui {

/**
 * This draws a horizontal or vertical bar/column chart. For two dimensional
 * series this draws one bars for each point in the series with label X and
 * height Y. For three dimensional series this draws one bar for each point in
 * the series with label X, lower bound Y and upper bound Z.
 *
 * If points share the same labels in all series they will be shown as grouped
 * bars by series. If stacked is true, all bars in a group will be stacked on
 * top of each other.
 *
 * OPTIONS
 *
 *   orientation        = {horizontal,vertical}, default: horizontal
 *   stacked            = {on,off}, default: off
 */
template <typename TX, typename TY>
class BarChart : public Drawable {
public:
  enum kBarChartOrientation {
    O_VERTICAL,
    O_HORIZONTAL
  };

  constexpr static const double kBarPadding = 0.2f; // FIXPAUL make configurable

  /**
   * Create a new bar chart with an explicit y domain
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param orientation one of {O_HORIZNTAL,O_VERTICAL}. default is horizontal
   * @param stack groups?
   * @param the y/value domain. does not transfer ownership
   */
  BarChart(
      Canvas* canvas,
      Domain<TX>* x_domain,
      Domain<TY>* y_domain,
      kBarChartOrientation orientation = O_HORIZONTAL,
      bool stacked = false);

  /**
   * Add a (x: string, y: double) series. This will draw one bar for each point
   * in the series where x is the label of the bar and y is the height of the
   * bar
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series2D<TX, TY>* series);

  /**
   * Add an axis to the chart. This method should only be called after all
   * series have been added to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param position the position/placement of the axis
   */
   AxisDefinition* addAxis(AxisDefinition::kPosition position) override;

protected:

  void render(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const override;

  void renderVerticalBars(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const;

  void renderHorizontalBars(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const;

  struct BarData {
    TX x;
    std::vector<std::pair<double, TY>> ys;
  };

  Canvas* canvas_;
  kBarChartOrientation orientation_;
  bool stacked_;
  Domain<TX>* x_domain_;
  Domain<TY>* y_domain_;
  std::vector<BarData> data_;
  int num_series_;
  std::vector<std::string> series_colors_;
};

template <typename TX, typename TY>
BarChart<TX, TY>::BarChart(
    Canvas* canvas,
    Domain<TX>* x_domain,
    Domain<TY>* y_domain,
    kBarChartOrientation orientation /* = O_HORIZONTAL */,
    bool stacked /* = false */) :
    canvas_(canvas),
    x_domain_(x_domain),
    y_domain_(y_domain),
    orientation_(orientation),
    stacked_(stacked),
    num_series_(0) {}


template <typename TX, typename TY>
void BarChart<TX, TY>::addSeries(Series2D<TX, TY>* series) {
  series_colors_.emplace_back(seriesColor(series));

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

    if (bar_data->ys.size() < num_series_ + 1) {
      for (int i = bar_data->ys.size(); i < num_series_; ++i) {
        bar_data->ys.emplace_back(0, 0);
      }

      // bar_data->ys.emplace_back(0, y_val);
    }
  }

  num_series_++;
}

template <typename TX, typename TY>
AxisDefinition*  BarChart<TX, TY>::addAxis(AxisDefinition::kPosition position) {
  switch (position) {

    case AxisDefinition::TOP:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, x_domain_);
        case O_HORIZONTAL:
          return canvas_->addAxis(position, y_domain_);
      }

    case AxisDefinition::RIGHT:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, y_domain_);
        case O_HORIZONTAL:
          return canvas_->addAxis(position, x_domain_);
      }

    case AxisDefinition::BOTTOM:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, x_domain_);
        case O_HORIZONTAL:
          return canvas_->addAxis(position, y_domain_);
      }

    case AxisDefinition::LEFT:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, y_domain_);
        case O_HORIZONTAL:
          return canvas_->addAxis(position, x_domain_);
      }

  }
}

template <typename TX, typename TY>
void BarChart<TX, TY>::render(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  if (data_.size() == 0) {
    RAISE(util::RuntimeException, "BarChart#render called without any data");
  }

  switch (orientation_) {
    case O_VERTICAL:
      target->beginGroup("bars vertical");
      renderVerticalBars(target, width, height, padding);
      target->finishGroup();
      break;
    case O_HORIZONTAL:
      target->beginGroup("bars horizontal");
      renderHorizontalBars(target, width, height, padding);
      target->finishGroup();
      break;
  }
}

template <typename TX, typename TY>
void BarChart<TX, TY>::renderVerticalBars(
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
  auto y_domain = y_domain_;
  for (const auto& bar : data_) {
    draw_x += bar_padding;

    /* single series */
    if (num_series_ == 1) {
      //auto y_min = y_domain->scale(bar.ys[0].first);
      double y_min = 0; // FIXPAUL
      auto y_max = y_domain->scale(bar.ys[0].second);
      auto draw_y = padding_top + ((1.0f - y_max) * inner_height);
      auto draw_height = (1.0f - ((1.0f - y_max) + y_min)) * inner_height;
      target->drawRect(
          draw_x,
          draw_y,
          draw_width,
          draw_height,
          series_colors_[0],
          "bar");
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

        target->drawRect(
            draw_x,
            draw_y,
            draw_width,
            draw_height,
            series_colors_[i],
            "bar");

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
            series_colors_[i],
            "bar");

        draw_x_multi += (draw_width_multi * (1.0f + kBarPadding * 0.5f));
      }
    }

    draw_x += bar_width + bar_padding;
  }
}

template <typename TX, typename TY>
void BarChart<TX, TY>::renderHorizontalBars(
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
  auto y_domain = y_domain_;
  for (const auto& bar : data_) {
    draw_y += bar_padding;

    /* single series */
    if (num_series_ == 1) {
      auto& y_val = bar.ys[0];
      //auto y_min = y_domain->scale(bar.ys[0].first);
      double y_min = 0.0f; // FIXPAUL
      auto y_max = y_domain->scale(bar.ys[0].second);
      auto draw_x = padding_left + y_min * inner_width;
      auto draw_width = (y_max - y_min) * inner_width;

      target->drawRect(
          draw_x,
          draw_y,
          draw_width,
          draw_height, 
          series_colors_[0],
          "bar");
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

        target->drawRect(
            draw_x,
            draw_y,
            draw_width,
            draw_height,
            series_colors_[i],
            "bar");

        y_min += y_val.second - y_val.first;
      }
    }

    /* multi series unstacked */
    else {
      auto draw_y_multi = draw_y;
      auto draw_height_multi = draw_height / num_series_;
      for (int i = 0; i < bar.ys.size(); i++) {
        //auto y_min = y_domain->scale(bar.ys[i].first);
        //auto y_min = y_domain->scale(bar.ys[i].first);
        double y_min = 0;
        auto y_max = y_domain->scale(bar.ys[i].second);
        auto draw_x = padding_left + y_min * inner_width;
        auto draw_width = (y_max - y_min) * inner_width;

        target->drawRect(
            draw_x,
            draw_y_multi,
            draw_width,
            draw_height_multi * (1.0f - kBarPadding * 0.5f),
            series_colors_[i],
            "bar");

        draw_y_multi += (draw_height_multi * (1.0f + kBarPadding * 0.5f));
      }
    }

    draw_y += bar_height + bar_padding;
  }
}



}
}
#endif

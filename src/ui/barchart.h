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
template <typename TX, typename TY, typename TZ>
class BarChart3D : public Drawable {
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
  BarChart3D(Canvas* canvas);

  /**
   * Add a (x: string, y: double) series. This will draw one bar for each point
   * in the series where x is the label of the bar and y is the height of the
   * bar
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series3D<TX, TY, TZ>* series);

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

  void render(RenderTarget* target, Viewport* viewport) const override;

/*
  void renderVerticalBars(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const;

*/
  void renderHorizontalBars(RenderTarget* target, Viewport* viewport) const;

  struct BarData {
    BarData(const Series::Point<TX>& x_) : x(x_) {}
    Series::Point<TX> x;
    std::vector<std::pair<Series::Point<TY>, Series::Point<TZ>>> ys;
  };
  std::vector<BarData> data_;

  //DomainAdapter x_domain_;
  //DomainAdapter y_domain_;

  Canvas* canvas_;
  kBarChartOrientation orientation_;
  bool stacked_;
  int num_series_;
};

template <typename TX, typename TY, typename TZ>
BarChart3D<TX, TY, TZ>::BarChart3D(
    Canvas* canvas) :
    canvas_(canvas),
    num_series_(0) {}

// FIXPAUL enforce that TY == TZ
template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::addSeries(Series3D<TX, TY, TZ>* series) {
  //series_colors_.emplace_back(seriesColor(series));

  for (const auto& point : series->getData()) {
    const auto& x_val = std::get<0>(point);
    const auto& y_val = std::get<1>(point);
    const auto& z_val = std::get<2>(point);

    printf("adddata: y=%s,z=%s\n",
        std::to_string(y_val.value()).c_str(),
        std::to_string(z_val.value()).c_str());

    BarData* bar_data = nullptr;
    for (auto& candidate : data_) {
      if (candidate.x == x_val) {
        bar_data = &candidate;
      }
    }

    if (bar_data == nullptr) {
      data_.emplace_back(x_val);
      bar_data = &data_.back();
    }

    if (bar_data->ys.size() < num_series_ + 1) {
      for (int i = bar_data->ys.size(); i < num_series_; ++i) {
      //  bar_data->ys.emplace_back(0, 0);
      }

      bar_data->ys.emplace_back(y_val, z_val);
    }
  }

  num_series_++;
}

template <typename TX, typename TY, typename TZ>
AxisDefinition*  BarChart3D<TX, TY, TZ>::addAxis(AxisDefinition::kPosition position) {
  /*
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
  */
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::render(
    RenderTarget* target,
    Viewport* viewport) const {
  if (data_.size() == 0) {
    RAISE(util::RuntimeException, "BarChart3D#render called without any data");
  }

  //switch (O_VERTICAL) {
  //  case O_VERTICAL:
  //    target->beginGroup("bars vertical");
  //    renderVerticalBars(target, width, height, padding);
  //    target->finishGroup();
  //    break;
    //case O_HORIZONTAL:
      renderHorizontalBars(target, viewport);
      //break;
  //}
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::renderHorizontalBars(
    RenderTarget* target,
    Viewport* viewport) const {
  /* calculate bar width and padding */
  //auto bar_height = (inner_height / data_.size()) * (1.0f - kBarPadding);
  //auto bar_padding = (inner_height / data_.size()) * (kBarPadding * 0.5f);
  //bar_height -= bar_padding / data_.size() * 2;

  /* draw the bars */
  //std::vector<double> y_ticks = {0.0f};
  //std::vector<std::pair<double, std::string>> y_labels;

  //auto draw_y = padding_top + bar_padding;
  //auto draw_height = bar_height;
  //auto y_domain = y_domain_;

  //target->beginGroup("bars horizontal");
  //target->finishGroup();

  /* prepare x domain --- FIXPAUL move somewhere else */
  CategoricalDomain<TX> x_domain;
  for (const auto& bar : data_) {
    x_domain.addCategory(bar.x.value());
  }

  NumericalDomain<TY> y_domain;
  for (const auto& bar : data_) {
    y_domain.addValue(bar.ys[0].first.value());
    y_domain.addValue(static_cast<TY>(bar.ys[0].second.value()));
  }

  //axes->emplace_back(AxisDefinition::LEFT, &x_domain);

  for (const auto& bar : data_) {
    //draw_y += bar_padding;
    auto x = x_domain.scaleRange(bar.x.value());
    //printf("x: %f - %f\n", x.first, x.second);

    //if (num_series_ == 1) {

    printf("renderdata: y=%s,z=%s\n",
        std::to_string(bar.ys[0].first.value()).c_str(),
        std::to_string(bar.ys[0].second.value()).c_str());

      auto y_min = y_domain.scale(bar.ys[0].first.value());
      auto y_max = y_domain.scale(static_cast<TY>(bar.ys[0].second.value()));

      printf("y: %f - %f\n", y_min, y_max);
      if (!(y_min <= y_max)) { // doubles are funny...
        RAISE(
            util::RuntimeException,
            "BarChart error: invalid point in series. Z value must be greater "
            "or equal to Y value for all points");
      }

      auto dx = viewport->paddingLeft() + y_min * viewport->innerWidth();
      auto dy = viewport->paddingTop() + x.first * viewport->innerHeight();
      auto dw = (y_max - y_min) * viewport->innerWidth();
      auto dh = (x.second - x.first) * viewport->innerHeight();

      target->drawRect(dx, dy, dw, dh, "#000000", "bar");
    //}

    /* stacked */
    /*else if (stacked_) {
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
    }*/

    /* multi series unstacked */
    /*else {
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
    }*/

    //draw_y += bar_height + bar_padding;
  }
}

}
}
#endif

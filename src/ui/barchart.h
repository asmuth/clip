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
#include <fnordmetric/ui/seriesjoin.h>
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

  constexpr static const double kBarPadding = 0.3f; // FIXPAUL make configurable
  constexpr static const double kBarPaddingInner = 0.2f; // FIXPAUL make configurable

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
  void renderHorizontalBars(
      RenderTarget* target,
      Viewport* viewport,
      SeriesJoin3D<TX, TY, TZ> const* data) const;

  void stackData(SeriesJoin3D<TX, TY, TZ> const* target) const;

  DomainAdapter x_domain_;
  DomainAdapter y_domain_;
  SeriesJoin3D<TX, TY, TY> data_;

  kBarChartOrientation orientation_;
  bool stacked_;
};

template <typename TX, typename TY, typename TZ>
BarChart3D<TX, TY, TZ>::BarChart3D(
    Canvas* canvas) :
    Drawable(canvas),
    orientation_(O_HORIZONTAL),
    stacked_(true) {}

// FIXPAUL enforce that TY == TZ
template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::addSeries(Series3D<TX, TY, TZ>* series) {
  Domain<TX>* x_domain;
  if (x_domain_.empty()) {
    x_domain = new DiscreteDomain<TX>();
    x_domain_.reset(x_domain, true);
  } else {
    x_domain = x_domain_.getAs<Domain<TX>>();
  }

  Domain<TY>* y_domain;
  if (y_domain_.empty()) {
    y_domain = new ContinuousDomain<TY>(); // FIXPAUL domain factory!
    y_domain_.reset(y_domain, true);
  } else {
    y_domain = y_domain_.getAs<Domain<TY>>();
  }

  for (const auto& point : series->getData()) {
    x_domain->addValue(std::get<0>(point).value());
    y_domain->addValue(std::get<1>(point).value());
    y_domain->addValue(static_cast<TY>(std::get<2>(point).value()));
  }

  data_.addSeries(series);
}

template <typename TX, typename TY, typename TZ>
AxisDefinition* BarChart3D<TX, TY, TZ>::addAxis(
    AxisDefinition::kPosition position) {
  auto axis = canvas_->addAxis(position);

  switch (position) {

    case AxisDefinition::TOP:
      switch (orientation_) {
        case O_VERTICAL:
          axis->setDomain(&x_domain_);
          return axis;
        case O_HORIZONTAL:
          axis->setDomain(&y_domain_);
          return axis;
      }

    case AxisDefinition::RIGHT:
      switch (orientation_) {
        case O_VERTICAL:
          axis->setDomain(&y_domain_);
          return axis;
        case O_HORIZONTAL:
          axis->setDomain(&x_domain_);
          return axis;
      }

    case AxisDefinition::BOTTOM:
      switch (orientation_) {
        case O_VERTICAL:
          axis->setDomain(&x_domain_);
          return axis;
        case O_HORIZONTAL:
          axis->setDomain(&y_domain_);
          return axis;
      }

    case AxisDefinition::LEFT:
      switch (orientation_) {
        case O_VERTICAL:
          axis->setDomain(&y_domain_);
          return axis;
        case O_HORIZONTAL:
          axis->setDomain(&x_domain_);
          return axis;
      }

  }
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::render(
    RenderTarget* target,
    Viewport* viewport) const {
  if (data_.size() == 0) {
    RAISE(util::RuntimeException, "BarChart3D#render called without any data");
  }

  SeriesJoin3D<TX, TY, TZ> const* data;
  SeriesJoin3D<TX, TY, TZ> stacked_data;
  if (stacked_) {
    stackData(&stacked_data);
    data = &stacked_data;
  } else {
    data = &data_;
  }

  switch (orientation_) {
  //  case O_VERTICAL:
  //    target->beginGroup("bars vertical");
  //    renderVerticalBars(target, width, height, padding);
  //    target->finishGroup();
  //    break;
    case O_HORIZONTAL:
  //target->beginGroup("bars horizontal");
  //target->finishGroup();
      renderHorizontalBars(target, viewport, data);
      break;
  }
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::renderHorizontalBars(
    RenderTarget* target,
    Viewport* viewport,
    SeriesJoin3D<TX, TY, TZ> const* data) const {
  auto x_domain = x_domain_.getAs<Domain<TX>>();
  auto y_domain = y_domain_.getAs<Domain<TY>>();

  for (const auto& bar : data->getData()) {
    auto x = x_domain->scaleRange(bar.x.value());

    for (int n = 0; n < data->seriesCount(); n++) {
      auto y_min = y_domain->scale(bar.ys[n].first.value());
      auto y_max = y_domain->scale(static_cast<TY>(bar.ys[n].second.value()));

      if (!(y_min <= y_max)) { // doubles are funny...
        RAISE(
            util::RuntimeException,
            "BarChart error: invalid point in series. Z value must be greater "
            "or equal to Y value for all points");
      }

      auto dw = (y_max - y_min) * viewport->innerWidth();
      auto dh = (x.second - x.first) * viewport->innerHeight();
      auto dx = viewport->paddingLeft() + y_min * viewport->innerWidth();
      auto dy = viewport->paddingTop() +
          (1.0 - x.first) * viewport->innerHeight() - dh;

      dy += dh * kBarPadding * 0.5;
      dh *= (1.0 - kBarPadding);

      if (!stacked_) {
        dh /= data->seriesCount();
        dy += dh * n + (dh * kBarPaddingInner * 0.5);
        dh *= (1.0 - kBarPaddingInner);
      }

      target->drawRect(dx, dy, dw, dh, "#000000", "bar");
    }
  }
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::stackData(
    SeriesJoin3D<TX, TY, TZ> const* target) const {

}


}
}
#endif

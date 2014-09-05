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
#include <fnordmetric/ui/colorpalette.h>
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
class BarChart : public Drawable {
public:
  constexpr static const double kBarPadding = 0.3f; // FIXPAUL make configurable
  constexpr static const double kBarPaddingInner = 0.2f; // FIXPAUL make configurable

  enum kBarChartOrientation {
    O_VERTICAL,
    O_HORIZONTAL
  };

  /**
   * Create a new bar chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   */
  BarChart(Canvas* canvas);

  /**
   * Set the orientation of this bar chart
   *
   * @param orientation one of {O_HORIZONTAL, O_VERTICAL}
   */
  void setOrientation(kBarChartOrientation orientation);

  /**
   * Set the stacked property of this bar chart
   *
   * @param stack bars? true or false
   */
  virtual void setStacked(bool stacked) = 0;

protected:
  kBarChartOrientation orientation_;
  bool stacked_;
};

template <typename TX_, typename TY_, typename TZ_>
class BarChart3D : public BarChart {
public:
  typedef TX_ TX;
  typedef TY_ TY;
  typedef TZ_ TZ;

  /**
   * Create a new bar chart for 3 dimensional series
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   */
  BarChart3D(Canvas* canvas);

  /**
   * Add a (x: string, y: double, z: double) series. This will draw one bar for
   * each point in the series where x is the label of the bar, y is the lower
   * bound of the bar and z is the upper bound of the bar
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series3D<TX, TY, TZ>* series);

  /**
   * Add an axis to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param position the position/placement of the axis
   */
   AxisDefinition* addAxis(AxisDefinition::kPosition position) override;

  /**
   * Set the stacked property of this bar chart
   *
   * @param stack bars? true or false
   */
  void setStacked(bool stacked) override;

  /**
   * Get the {x,y,z} domain of this chart.
   *
   * The returned pointer is owned by the chart object and must not be freed
   * by the caller!
   *
   * @param dimension the dimension (x,y,z)
   */
  AnyDomain* getDomain(AnyDomain::kDimension dimension) override;

protected:

  void render(RenderTarget* target, Viewport* viewport) const override;

  void renderVerticalBars(
      RenderTarget* target,
      Viewport* viewport,
      SeriesJoin3D<TX, TY, TZ> const* data) const;

  void renderHorizontalBars(
      RenderTarget* target,
      Viewport* viewport,
      SeriesJoin3D<TX, TY, TZ> const* data) const;

  void stackData(SeriesJoin3D<TX, TY, TZ>* target) const;
  const std::string& seriesColor(size_t series_index) const;

  DomainAdapter x_domain_;
  DomainAdapter y_domain_;
  SeriesJoin3D<TX, TY, TY> data_;
  std::vector<Series3D<TX, TY, TY>*> series_;
  ColorPalette color_palette_;
};

template <typename TX_, typename TY_>
class BarChart2D : public BarChart3D<TX_, TY_, TY_> {
public:
  typedef TX_ TX;
  typedef TY_ TY;

  /**
   * Create a new bar chart for 2 dimensional series
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   */
  BarChart2D(Canvas* canvas);

  /**
   * Add a (x: string, y: double) series. This will draw one bar for each point
   * in the series where x is the label of the bar and y is the height of the
   * bar
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series2D<TX, TY>* series);

};

template <typename TX, typename TY, typename TZ>
BarChart3D<TX, TY, TZ>::BarChart3D(Canvas* canvas) : BarChart(canvas) {}

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
    y_domain = Domain<TY>::mkDomain();
    y_domain_.reset(y_domain, true);
  } else {
    y_domain = y_domain_.getAs<Domain<TY>>();
  }

  for (const auto& point : series->getData()) {
    x_domain->addValue(point.x());
    y_domain->addValue(point.y());
    y_domain->addValue(static_cast<TY>(point.z()));

    if (!(point.y() <= point.z())) {
      RAISE(
          util::RuntimeException,
          "BarChart error: invalid point in series. Z value must be greater "
          "or equal to Y value for all points");
    }

    if (series->hasProperty(Series::P_LABEL)) {
      printf("label=%s\n", series->getProperty(Series::P_LABEL, &point).c_str());
    }
  }

  data_.addSeries(series);
  series_.push_back(series);
  setStacked(stacked_);

  if (!series->hasProperty(Series::P_COLOR)) {
    color_palette_.setNextColor(series);
  }
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
    case O_VERTICAL:
      target->beginGroup("bars vertical");
      renderVerticalBars(target, viewport, data);
      target->finishGroup();
      break;
    case O_HORIZONTAL:
      target->beginGroup("bars horizontal");
      renderHorizontalBars(target, viewport, data);
      target->finishGroup();
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

      target->drawRect(dx, dy, dw, dh, seriesColor(n), "bar");
    }
  }
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::renderVerticalBars(
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

      auto dw = (x.second - x.first) * viewport->innerWidth();
      auto dh = (y_max - y_min) * viewport->innerHeight();
      auto dx = viewport->paddingLeft() + x.first * viewport->innerWidth();
      auto dy = viewport->paddingTop() +
          (1.0 - y_max) * viewport->innerHeight();

      dx += dw * kBarPadding * 0.5;
      dw *= (1.0 - kBarPadding);

      if (!stacked_) {
        dw /= data->seriesCount();
        dx += dw * n + (dw * kBarPaddingInner * 0.5);
        dw *= (1.0 - kBarPaddingInner);
      }

      target->drawRect(dx, dy, dw, dh, seriesColor(n), "bar");
    }
  }
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::stackData(
    SeriesJoin3D<TX, TY, TZ>* target) const {
  for (const auto& bar : data_.getData()) {
    TY cur = 0;

    for (const auto& y : bar.ys) {
      TY delta = y.second.value() - y.first.value();

      target->addPoint(
          typename Series3D<TX, TY, TZ>::Point(
              bar.x,
              Series::Coord<TY>(cur),
              Series::Coord<TY>(cur + delta)), true);

      cur += delta;
    }
  }

  target->setSeriesCount(data_.seriesCount());
}

template <typename TX, typename TY, typename TZ>
const std::string& BarChart3D<TX, TY, TZ>::seriesColor(
    size_t series_index) const {
  if (series_index > series_.size()) {
    RAISE(util::RuntimeException, "invalid series index");
  }

  return series_[series_index]->getProperty(Series::P_COLOR);
}

template <typename TX, typename TY, typename TZ>
void BarChart3D<TX, TY, TZ>::setStacked(bool stacked) {
  stacked_ = stacked;

  if (stacked && !y_domain_.empty()) {
    auto y_domain = y_domain_.getAs<Domain<TY>>();

    for (const auto& bar : data_.getData()) {
      TY max = 0;

      for (const auto& y : bar.ys) {
        max += y.second.value() - y.first.value();
      }

      y_domain->addValue(max);
    }
  }
}

template <typename TX, typename TY, typename TZ>
AnyDomain* BarChart3D<TX, TY, TZ>::getDomain(AnyDomain::kDimension dimension) {
  switch (dimension) {
    case AnyDomain::DIM_X:
      return x_domain_.get();

    case AnyDomain::DIM_Y:
    case AnyDomain::DIM_Z:
      return y_domain_.get();
  }
}

template <typename TX, typename TY>
BarChart2D<TX, TY>::BarChart2D(
    Canvas* canvas) :
    BarChart3D<TX, TY, TY>(canvas) {}

template <typename TX, typename TY>
void BarChart2D<TX, TY>::addSeries(Series2D<TX, TY>* series) {
  auto series3d = new Series3D<TX, TY, TY>(); // FIXPAUL: never free'd!
  auto copy_labels = series->hasProperty(Series::P_LABEL);

  for (const auto& point : series->getData()) {
    if (point.y() > 0) {
      series3d->addDatum(
          Series::Coord<TX>(point.x()),
          Series::Coord<TY>(nullptr),
          Series::Coord<TY>(point.y()));
    } else {
      series3d->addDatum(
          Series::Coord<TX>(point.x()),
          Series::Coord<TY>(point.y()),
          Series::Coord<TY>(nullptr));
    }

    if (copy_labels) {
      series3d->setProperty(
          Series::P_LABEL,
          &series3d->getData().back(),
          series->getProperty(Series::P_LABEL, &point));
    }
  }

  if (series->hasProperty(Series::P_COLOR)) {
    series3d->setDefaultProperty(
        Series::P_COLOR,
        series->getProperty(Series::P_COLOR));
  }

  BarChart3D<TX, TY, TY>::addSeries(series3d);
}

}
}
#endif

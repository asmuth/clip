/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_POINTCHART_H
#define _FNORDMETRIC_POINTCHART_H
#include <stdlib.h>
#include <assert.h>
#include <fnordmetric/base/series.h>
#include <fnordmetric/ui/axisdefinition.h>
#include <fnordmetric/ui/domain.h>
#include <fnordmetric/ui/drawable.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/colorpalette.h>
#include <fnordmetric/ui/rendertarget.h>

namespace fnordmetric {
namespace ui {

/**
 * This draws the series data as points.
 */
class PointChart : public Drawable {
public:
  static char kDefaultPointStyle[];
  static char kDefaultPointSize[];

  PointChart(ui::Canvas* canvas);
};

template <typename TX_, typename TY_, typename TZ_>
class PointChart3D : public PointChart {
public:
  typedef TX_ TX;
  typedef TY_ TY;
  typedef TZ_ TZ;

  /**
   * Create a new point chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   */
  PointChart3D(Canvas* canvas);

  /**
   * Create a new point chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  PointChart3D(Canvas* canvas, AnyDomain* x_domain, AnyDomain* y_domain);

  /**
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
   AxisDefinition* addAxis(AxisDefinition::kPosition position);

  /**
   * Get the {x,y} domain of this chart. Will raise an exception if z domain
   * is requested.
   *
   * The returned pointer is owned by the chart object and must not be freed
   * by the caller!
   *
   * @param dimension the dimension (x,y)
   */
  AnyDomain* getDomain(AnyDomain::kDimension dimension) override;

protected:

  void render(
      RenderTarget* target,
      Viewport* viewport) const override;

  DomainAdapter x_domain_;
  DomainAdapter y_domain_;
  std::vector<Series3D<TX, TY, TZ>*> series_;
  ColorPalette color_palette_;
};

template <typename TX_, typename TY_>
class PointChart2D : public PointChart3D<TX_, TY_, double> {
public:
  typedef TX_ TX;
  typedef TY_ TY;

  /**
   * Create a new point chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   */
  PointChart2D(Canvas* canvas);

  /**
   * Create a new point chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  PointChart2D(Canvas* canvas, AnyDomain* x_domain, AnyDomain* y_domain);

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
PointChart3D<TX, TY, TZ>::PointChart3D(Canvas* canvas) : PointChart(canvas) {}

template <typename TX, typename TY, typename TZ>
PointChart3D<TX, TY, TZ>::PointChart3D(
    Canvas* canvas,
    AnyDomain* x_domain,
    AnyDomain* y_domain) :
    PointChart(canvas) {
  x_domain_.reset(x_domain);
  y_domain_.reset(y_domain);
}

template <typename TX, typename TY, typename TZ>
void PointChart3D<TX, TY, TZ>::addSeries(Series3D<TX, TY, TZ>* series) {
  Domain<TX>* x_domain;
  if (x_domain_.empty()) {
    x_domain = Domain<TX>::mkDomain();
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
  }

  series_.push_back(series);

  if (!series->hasProperty(Series::P_COLOR)) {
    color_palette_.setNextColor(series);
  }

  series->setDefaultProperty(
      Series::P_POINT_STYLE,
      PointChart::kDefaultPointStyle);
}

template <typename TX, typename TY, typename TZ>
void PointChart3D<TX, TY, TZ>::render(
    RenderTarget* target,
    Viewport* viewport) const {
  target->beginGroup("points");

  for (const auto& series : series_) {
    std::vector<std::pair<double, double>> coords;

    // FIXPAUL catch conversion excpetion
    auto point_style = series->getProperty(Series::P_POINT_STYLE);
    auto color = series->getProperty(Series::P_COLOR);

    for (const auto& point : series->getData()) {
      auto x = x_domain_.getAs<Domain<TX>>()->scale(point.x());
      auto y = y_domain_.getAs<Domain<TY>>()->scale(point.y());
      auto ss_x = viewport->paddingLeft() + x * viewport->innerWidth();
      auto ss_y = viewport->paddingTop() + (1.0 - y) * viewport->innerHeight();

      if (point_style != "none") {
        target->drawPoint(
            ss_x,
            ss_y,
            point_style,
            point.z(),
            color,
            "point");
      }
    }
  }

  target->finishGroup();
}

template <typename TX, typename TY, typename TZ>
AxisDefinition* PointChart3D<TX, TY, TZ>::addAxis(
    AxisDefinition::kPosition position) {
  auto axis = canvas_->addAxis(position);

  switch (position) {

    case AxisDefinition::TOP:
      axis->setDomain(&x_domain_);
      return axis;

    case AxisDefinition::RIGHT:
      axis->setDomain(&y_domain_);
      return axis;

    case AxisDefinition::BOTTOM:
      axis->setDomain(&x_domain_);
      return axis;

    case AxisDefinition::LEFT:
      axis->setDomain(&y_domain_);
      return axis;

  }
}

template <typename TX, typename TY, typename TZ>
AnyDomain* PointChart3D<TX, TY, TZ>::getDomain(
    AnyDomain::kDimension dimension) {
  switch (dimension) {
    case AnyDomain::DIM_X:
      return x_domain_.get();

    case AnyDomain::DIM_Y:
      return y_domain_.get();

    case AnyDomain::DIM_Z:
      RAISE(util::RuntimeException, "PointChart3D does not have a Z domain");
      return nullptr;
  }
}

template <typename TX, typename TY>
PointChart2D<TX, TY>::PointChart2D(
    Canvas* canvas) :
    PointChart3D<TX, TY, double>(canvas) {}

template <typename TX, typename TY>
PointChart2D<TX, TY>::PointChart2D(
    Canvas* canvas,
    AnyDomain* x_domain,
    AnyDomain* y_domain) :
    PointChart3D<TX, TY, double>(canvas, x_domain, y_domain) {}

template <typename TX, typename TY>
void PointChart2D<TX, TY>::addSeries(Series2D<TX, TY>* series) {
  auto series3d = new Series3D<TX, TY, double>(); // FIXPAUL: never free'd!
  auto copy_labels = series->hasProperty(Series::P_LABEL);

  series->setDefaultProperty(
      Series::P_POINT_SIZE,
      PointChart::kDefaultPointSize);

  // FIXPAUL copy point style
  for (const auto& point : series->getData()) {
    // FIXPAUL catch converison errror
    double point_size = std::stod(
        series->getProperty(Series::P_POINT_SIZE, &point));

    series3d->addDatum(
        Series::Coord<TX>(point.x()),
        Series::Coord<TY>(point.y()),
        Series::Coord<double>(point_size));

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

  PointChart3D<TX, TY, TY>::addSeries(series3d);
}

}
}
#endif

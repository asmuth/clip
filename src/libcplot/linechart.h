/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_LINECHART_H
#define _libstx_LINECHART_H
#include <stdlib.h>
#include "cplot/axisdefinition.h"
#include "cplot/continuousdomain.h"
#include "cplot/domain.h"
#include "cplot/drawable.h"
#include "cplot/canvas.h"
#include "cplot/colorpalette.h"
#include "cplot/rendertarget.h"

namespace stx {
namespace chart {

/**
 * This draws the series data as points.
 *
 * OPTIONS
 *
 *   line_style = {solid,dashed}, default: solid
 *   line_width = default: 2
 *
 */
class LineChart : public Drawable {
public:
  static const constexpr int kLabelPadding = 8;
  static char kDefaultLineStyle[];
  static char kDefaultLineWidth[];
  static char kDefaultPointStyle[];
  static char kDefaultPointSize[];

  LineChart(chart::Canvas* canvas);

  void setLabels(bool show_labels);

protected:
  bool show_labels_;
};

template <typename TX_, typename TY_>
class LineChart2D : public LineChart {
public:
  typedef TX_ TX;
  typedef TY_ TY;

  /**
   * Create a new line chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  LineChart2D(Canvas* canvas);

  /**
   * Create a new line chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  LineChart2D(Canvas* canvas, AnyDomain* x_domain, AnyDomain* y_domain);

  /**
   * Add a (x: string, y: double) series. This will draw one connected line
   * through all points in the series
   *
   * @param series the series to add. does not transfer ownership
   * @param line_width the line widht
   * @param smooth smooth this line?
   * @param line_style the line style ({solid,dashed})
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
   AxisDefinition* addAxis(AxisDefinition::kPosition position);

  /**
   * Add a grid to the chart.
   *
   * The returned pointer is owned by the canvas object and must not be freed
   * by the caller!
   *
   * @param vertical render vertical grid lines?
   * @param horizontal render horizonyal grid lines?
   */
  GridDefinition* addGrid(GridDefinition::kPlacement placement) override;

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

  DomainProvider x_domain_;
  DomainProvider y_domain_;
  std::vector<Series2D<TX, TY>*> series_;
  ColorPalette color_palette_;
};

template <typename TX, typename TY>
LineChart2D<TX, TY>::LineChart2D(Canvas* canvas) : LineChart(canvas) {}

template <typename TX, typename TY>
LineChart2D<TX, TY>::LineChart2D(
    Canvas* canvas,
    AnyDomain* x_domain,
    AnyDomain* y_domain) :
    LineChart(canvas) {
  x_domain_.reset(x_domain);
  y_domain_.reset(y_domain);
}

template <typename TX, typename TY>
void LineChart2D<TX, TY>::addSeries(Series2D<TX, TY>* series) {
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

    auto cont = dynamic_cast<AnyContinuousDomain*>(y_domain);
    if (cont != nullptr) {
      cont->setPadding(
          AnyDomain::kDefaultDomainPadding,
          AnyDomain::kDefaultDomainPadding);
    }
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
      Series::P_LINE_STYLE,
      LineChart::kDefaultLineStyle);

  series->setDefaultProperty(
      Series::P_LINE_WIDTH,
      LineChart::kDefaultLineWidth);

  series->setDefaultProperty(
      Series::P_POINT_STYLE,
      LineChart::kDefaultPointStyle);

  series->setDefaultProperty(
      Series::P_POINT_SIZE,
      LineChart::kDefaultPointSize);

  Drawable::addSeries(series);
}

template <typename TX, typename TY>
void LineChart2D<TX, TY>::render(
    RenderTarget* target,
    Viewport* viewport) const {
  if (x_domain_.get() == nullptr || y_domain_.get() == nullptr) {
    RAISE(kRuntimeError, "could not build domains");
  }

  x_domain_.get()->build();
  y_domain_.get()->build();

  target->beginGroup("lines");

  for (const auto& series : series_) {
    std::vector<std::pair<double, double>> coords;

    auto point_style = series->getProperty(Series::P_POINT_STYLE);
    double point_size;
    auto line_style = series->getProperty(Series::P_LINE_STYLE);
    double line_width;

    try {
      line_width = std::stod(series->getProperty(Series::P_LINE_WIDTH));
    } catch (const std::exception& e) {
      RAISE(
          kRuntimeError,
          "invalid line width: %s",
          series->getProperty(Series::P_LINE_WIDTH).c_str());
    }

    try {
      point_size = std::stod(series->getProperty(Series::P_POINT_SIZE));
    } catch (const std::exception& e) {
      RAISE(
          kRuntimeError,
          "invalid point size: %s",
          series->getProperty(Series::P_POINT_SIZE).c_str());
    }

    auto color = series->getProperty(Series::P_COLOR);

    for (const auto& point : series->getData()) {
      auto x = x_domain_.getAs<Domain<TX>>()->scale(point.x());
      auto y = y_domain_.getAs<Domain<TY>>()->scale(point.y());
      auto ss_x = viewport->paddingLeft() + x * viewport->innerWidth();
      auto ss_y = viewport->paddingTop() + (1.0 - y) * viewport->innerHeight();
      auto label = series->labelFor(&point);

      if (show_labels_) {
        target->drawText(
            label,
            ss_x,
            ss_y - point_size - kLabelPadding,
            "middle",
            "text-after-edge",
            "label");
      }

      target->drawPoint(
          ss_x,
          ss_y,
          point_style,
          point_size,
          color,
          "point",
          label,
          series->name());

      coords.emplace_back(ss_x, ss_y);
    }

    target->drawPath(
        coords,
        line_style,
        line_width,
        false,
        color,
        "line");
  }

  target->finishGroup();
}

template <typename TX, typename TY>
AxisDefinition* LineChart2D<TX, TY>::addAxis(
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

template <typename TX, typename TY>
GridDefinition* LineChart2D<TX, TY>::addGrid(
    GridDefinition::kPlacement placement) {
  auto grid = canvas_->addGrid(placement);

  switch (placement) {

    case GridDefinition::GRID_VERTICAL:
      grid->setDomain(&y_domain_);
      return grid;

    case GridDefinition::GRID_HORIZONTAL:
      grid->setDomain(&x_domain_);
      return grid;

  }
}

template <typename TX, typename TY>
AnyDomain* LineChart2D<TX, TY>::getDomain(AnyDomain::kDimension dimension) {
  switch (dimension) {
    case AnyDomain::DIM_X:
      return x_domain_.get();

    case AnyDomain::DIM_Y:
      return y_domain_.get();

    case AnyDomain::DIM_Z:
      RAISE(kRuntimeError, "LineChart2D does not have a Z domain");
      return nullptr;
  }
}

}
}
#endif

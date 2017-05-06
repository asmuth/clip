/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_AREACHART_H
#define _libstx_AREACHART_H
#include <stdlib.h>
#include "cplot/axisdefinition.h"
#include "cplot/domain.h"
#include "cplot/continuousdomain.h"
#include "cplot/drawable.h"
#include "cplot/canvas.h"
#include "cplot/colorpalette.h"
#include "cplot/rendertarget.h"

namespace stx {
namespace chart {

class AreaChart : public Drawable {
public:
  static char kDefaultPointStyle[];
  static char kDefaultPointSize[];
  static char kDefaultLineStyle[];
  static char kDefaultLineWidth[];

  /**
   * Create a new area chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param stacked stack areas?
   */
  AreaChart(
      Canvas* canvas,
      bool stacked = false);

protected:
  bool stacked_;
};

template <typename TX_, typename TY_, typename TZ_>
class AreaChart3D : public AreaChart {
public:
  typedef TX_ TX;
  typedef TY_ TY;
  typedef TZ_ TZ;

  /**
   * Create a new area chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param stacked stack areas?
   */
  AreaChart3D(Canvas* canvas, bool stacked = false);

  /**
   * Create a new area chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   * @param stacked stack areas?
   */
  AreaChart3D(
      Canvas* canvas,
      AnyDomain* x_domain,
      AnyDomain* y_domain,
      bool stacked = false);

  /**
   * Add a (x: string, y: double, z: double) series. This will draw an area that
   * covers the surface between height y and z for each x.
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

  struct Area {
    std::string color;
    std::string line_style;
    std::string series;
    double line_width;
    std::string point_style;
    double point_size;
    std::vector<std::tuple<TX, TY, TZ>> points;
    std::vector<std::string> labels;
  };

  void render(
      RenderTarget* target,
      Viewport* viewport) const override;

  DomainProvider x_domain_;
  DomainProvider y_domain_;
  std::vector<Area> areas_;
  ColorPalette color_palette_;
};

template <typename TX_, typename TY_>
class AreaChart2D : public AreaChart3D<TX_, TY_, double> {
public:
  typedef TX_ TX;
  typedef TY_ TY;

  /**
   * Create a new area chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param stacked stack areas?
   */
  AreaChart2D(Canvas* canvas, bool stacked = false);

  /**
   * Create a new area chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   * @param stacked stack areas?
   */
  AreaChart2D(
      Canvas* canvas,
      AnyDomain* x_domain,
      AnyDomain* y_domain,
      bool stacked = false);

  /**
   * Add a (x: string, y: double) series. This will draw an area that covers
   * the surface between 0 and y for each x.
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series2D<TX, TY>* series);

};

template <typename TX, typename TY, typename TZ>
AreaChart3D<TX, TY, TZ>::AreaChart3D(
    Canvas* canvas,
    bool stacked /* = false */) : 
    AreaChart(canvas, stacked) {}

template <typename TX, typename TY, typename TZ>
AreaChart3D<TX, TY, TZ>::AreaChart3D(
    Canvas* canvas,
    AnyDomain* x_domain,
    AnyDomain* y_domain,
    bool stacked /* = false */) :
    AreaChart(canvas, stacked) {
  x_domain_.reset(x_domain);
  y_domain_.reset(y_domain);
}

template <typename TX, typename TY, typename TZ>
void AreaChart3D<TX, TY, TZ>::addSeries(Series3D<TX, TY, TZ>* series) {
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

  if (!series->hasProperty(Series::P_COLOR)) {
    color_palette_.setNextColor(series);
  }

  series->setDefaultProperty(
      Series::P_LINE_STYLE,
      AreaChart::kDefaultLineStyle);

  series->setDefaultProperty(
      Series::P_LINE_WIDTH,
      AreaChart::kDefaultLineWidth);

  series->setDefaultProperty(
      Series::P_POINT_STYLE,
      AreaChart::kDefaultPointStyle);

  series->setDefaultProperty(
      Series::P_POINT_SIZE,
      AreaChart::kDefaultPointSize);

  // FIXPAUL catch conversion errors
  Area area;
  area.series = series->name();
  area.color = series->getProperty(Series::P_COLOR);
  area.line_style = series->getProperty(Series::P_LINE_STYLE);
  area.point_style = series->getProperty(Series::P_POINT_STYLE);

  try {
    area.line_width = std::stod(series->getProperty(Series::P_LINE_WIDTH));
  } catch (const std::exception& e) {
    RAISE(
        kRuntimeError,
        "invalid line width: %s",
        series->getProperty(Series::P_LINE_WIDTH).c_str());
  }

  try {
    area.point_size = std::stod(series->getProperty(Series::P_POINT_SIZE));
  } catch (const std::exception& e) {
    RAISE(
        kRuntimeError,
        "invalid point size: %s",
        series->getProperty(Series::P_POINT_SIZE).c_str());
  }

  for (const auto& point : series->getData()) {
    x_domain->addValue(point.x());
    y_domain->addValue(point.y());
    y_domain->addValue(point.z());
    area.points.emplace_back(
        point.x(),
        point.y(),
        point.z());

    area.labels.emplace_back(
        series->labelFor(&point));
  }

  // FIXPAUL: stacked areas, missing data
  areas_.emplace_back(area);
  Drawable::addSeries(series);
}

template <typename TX, typename TY, typename TZ>
void AreaChart3D<TX, TY, TZ>::render(
    RenderTarget* target,
    Viewport* viewport) const {
  if (x_domain_.get() == nullptr || y_domain_.get() == nullptr) {
    RAISE(kRuntimeError, "could not build domains");
  }

  x_domain_.get()->build();
  y_domain_.get()->build();

  target->beginGroup("areas");

  for (const auto& area : areas_) {
    std::vector<std::pair<double, double>> area_coords;
    std::vector<std::pair<double, double>> border_top_coords;
    std::vector<std::pair<double, double>> border_bottom_coords;

    for (int i = 0; i < area.points.size(); ++i) {
      const auto& point = area.points[i];
      auto s_x = x_domain_.getAs<Domain<TX>>()->scale(std::get<0>(point));
      auto s_y2 = 1.0 - y_domain_.getAs<Domain<TY>>()->scale(
          std::get<2>(point));

      auto draw_x = viewport->paddingLeft() + s_x * viewport->innerWidth();
      auto draw_y2 = viewport->paddingTop() + s_y2 * viewport->innerHeight();

      area_coords.emplace_back(draw_x, draw_y2);
      border_top_coords.emplace_back(draw_x, draw_y2);

      target->drawPoint(
          draw_x,
          draw_y2,
          area.point_style,
          area.point_size,
          area.color,
          "point",
          area.labels[i],
          area.series);
    }

    for (int i = area.points.size() - 1; i >= 0; --i) {
      const auto& point = area.points[i];
      auto s_x = x_domain_.getAs<Domain<TX>>()->scale(std::get<0>(point));
      auto s_y1 = 1.0 - y_domain_.getAs<Domain<TY>>()->scale(
          std::get<1>(point));
      auto draw_x = viewport->paddingLeft() + s_x * viewport->innerWidth();
      auto draw_y1 = viewport->paddingTop() + s_y1 * viewport->innerHeight();

      area_coords.emplace_back(draw_x, draw_y1);

      if (std::get<1>(point) != 0) {
        border_bottom_coords.emplace_back(draw_x, draw_y1);

        target->drawPoint(
            draw_x,
            draw_y1,
            area.point_style,
            area.point_size,
            area.color,
            "point",
            area.labels[i],
            area.series);
      }
    }

    target->drawPath(
        area_coords,
        "fill",
        area.line_style == "none" ? 0 : area.line_width,
        false,
        area.color,
        "area");

    if (area.line_style != "none") {
      target->drawPath(
          border_top_coords,
          area.line_style,
          area.line_width,
          false,
          area.color,
          "line");

      if (border_bottom_coords.size() > 0) {
        target->drawPath(
            border_bottom_coords,
            area.line_style,
            area.line_width,
            false,
            area.color,
            "line");
      }
    }
  }


  target->finishGroup();
}

template <typename TX, typename TY, typename TZ>
AxisDefinition* AreaChart3D<TX, TY, TZ>::addAxis(
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
GridDefinition* AreaChart3D<TX, TY, TZ>::addGrid(
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

template <typename TX, typename TY, typename TZ>
AnyDomain* AreaChart3D<TX, TY, TZ>::getDomain(
    AnyDomain::kDimension dimension) {
  switch (dimension) {
    case AnyDomain::DIM_X:
      return x_domain_.get();

    case AnyDomain::DIM_Y:
      return y_domain_.get();

    case AnyDomain::DIM_Z:
      RAISE(kRuntimeError, "AreaChart3D does not have a Z domain");
      return nullptr;
  }
}

template <typename TX, typename TY>
AreaChart2D<TX, TY>::AreaChart2D(
    Canvas* canvas,
    bool stacked /* = false */) :
    AreaChart3D<TX, TY, TY>(canvas, stacked) {}

template <typename TX, typename TY>
AreaChart2D<TX, TY>::AreaChart2D(
    Canvas* canvas,
    AnyDomain* x_domain,
    AnyDomain* y_domain,
    bool stacked /* = false */) :
    AreaChart3D<TX, TY, TY>(canvas, x_domain, y_domain, stacked) {}

template <typename TX, typename TY>
void AreaChart2D<TX, TY>::addSeries(Series2D<TX, TY>* series) {
  auto series3d = new Series3D<TX, TY, TY>(); // FIXPAUL: never free'd!
  auto copy_labels = series->hasProperty(Series::P_LABEL);

  for (const auto& point : series->getData()) {
    series3d->addDatum(
        Series::Coord<TX>(point.x()),
        Series::Coord<TY>(nullptr),
        Series::Coord<TY>(point.y()));

    if (copy_labels) {
      series3d->setProperty(
          Series::P_LABEL,
          &series3d->getData().back(),
          series->getProperty(Series::P_LABEL, &point));
    } else {
        series3d->setProperty(
          Series::P_LABEL,
          &series3d->getData().back(),
          series->labelFor(&point));
    }
  }

  if (series->hasProperty(Series::P_COLOR)) {
    series3d->setDefaultProperty(
        Series::P_COLOR,
        series->getProperty(Series::P_COLOR));
  }

  if (series->hasProperty(Series::P_LINE_STYLE)) {
    series3d->setDefaultProperty(
        Series::P_LINE_STYLE,
        series->getProperty(Series::P_LINE_STYLE));
  }

  if (series->hasProperty(Series::P_LINE_WIDTH)) {
    series3d->setDefaultProperty(
        Series::P_LINE_WIDTH,
        series->getProperty(Series::P_LINE_WIDTH));
  }

  if (series->hasProperty(Series::P_POINT_STYLE)) {
    series3d->setDefaultProperty(
        Series::P_POINT_STYLE,
        series->getProperty(Series::P_POINT_STYLE));
  }

  if (series->hasProperty(Series::P_POINT_SIZE)) {
    series3d->setDefaultProperty(
        Series::P_POINT_SIZE,
        series->getProperty(Series::P_POINT_SIZE));
  }

  AreaChart3D<TX, TY, TY>::addSeries(series3d);
}

}
}
#endif

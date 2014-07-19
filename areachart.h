/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_AREACHART_H
#define _FNORDMETRIC_AREACHART_H
#include <stdlib.h>
#include <assert.h>
#include "../base/series.h"
#include "axisdefinition.h"
#include "domain.h"
#include "drawable.h"

namespace fnordmetric {
namespace ui {
class Canvas;
class Domain;

/**
 * This draws the series data as points.
 *
 * OPTIONS
 *
 *   point_style = {none,circle}; default: none
 *   point_size = default: 2
 *   border_style = {none,solid,dashed}, default: none
 *   border_width = default: 2
 *
 */
class AreaChart : public Drawable {
public:
  static char kDefaultPointStyle[];
  static double kDefaultPointSize;
  static char kDefaultBorderStyle[];
  static double kDefaultBorderWidth;

  /**
   * Create a new area chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param stacked stack areas?
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  AreaChart(
      Canvas* canvas,
      NumericalDomain* x_domain = nullptr,
      NumericalDomain* y_domain = nullptr,
      bool stacked = false);

  /**
   * Add a (x: string, y: double) series. This will draw an area that covers
   * the surface between height 0 and y for each x.
   *
   * @param series the series to add. does not transfer ownership
   * @param border_style the border style
   * @param border_width the line width
   * @param point_style the point style
   * @param point_width the point size
   * @param smooth smooth this area?
   * @param line_style the line style ({solid,dashed})
   */
  void addSeries(
      Series2D<double, double>* series,
      const std::string& border_style = kDefaultBorderStyle,
      double border_width = kDefaultBorderWidth,
      const std::string& point_style = kDefaultPointStyle,
      double point_size = kDefaultPointSize,
      bool smooth = false);

  /**
   * Add a (x: string, y: double, z: double) series. This will draw an area that
   * covers the surface between height y and z for each x.
   *
   * @param series the series to add. does not transfer ownership
   * @param border_style the border style
   * @param border_width the line width
   * @param point_style the point style
   * @param point_width the point size
   * @param smooth smooth this area?
   * @param line_style the line style ({solid,dashed})
   */
  void addSeries(
      Series3D<double, double, double>* series,
      const std::string& border_style = kDefaultBorderStyle,
      double border_width = kDefaultBorderWidth,
      const std::string& point_style = kDefaultPointStyle,
      double point_size = kDefaultPointSize,
      bool smooth = false);

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

protected:

  NumericalDomain* getXDomain() const;
  NumericalDomain* getYDomain() const;

  void render(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const override;

  struct Area {
    std::vector<std::tuple<double, double, double>> points;
    std::string border_style;
    double border_width;
    std::string point_style;
    double point_size;
    std::string color;
    bool smooth;
  };

  Canvas* canvas_;
  NumericalDomain* x_domain_;
  NumericalDomain* y_domain_;
  bool stacked_;
  int num_series_;
  std::vector<Area> areas_;
  mutable std::unique_ptr<NumericalDomain> x_domain_auto_;
  mutable std::unique_ptr<NumericalDomain> y_domain_auto_;
};

}
}
#endif

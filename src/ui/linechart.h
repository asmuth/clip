/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_LINECHART_H
#define _FNORDMETRIC_LINECHART_H
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
 *   line_style = {solid,dashed}, default: solid
 *   line_width = default: 2
 *
 */
class LineChart : public Drawable {
public:
  static char kDefaultLineStyle[];
  static double kDefaultLineWidth;
  static char kDefaultPointStyle[];
  static double kDefaultPointSize;

  /**
   * Create a new line chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  LineChart(
      Canvas* canvas,
      NumericalDomain* x_domain = nullptr,
      NumericalDomain* y_domain = nullptr);

  /**
   * Add a (x: string, y: double) series. This will draw one connected line
   * through all points in the series
   *
   * @param series the series to add. does not transfer ownership
   * @param line_width the line widht
   * @param smooth smooth this line?
   * @param line_style the line style ({solid,dashed})
   */
  void addSeries(
      Series2D<double, double>* series,
      const std::string& line_style = kDefaultLineStyle,
      double line_width = kDefaultLineWidth,
      const std::string& point_style = kDefaultPointStyle,
      double point_size = kDefaultPointSize,
      bool smooth = false);

  /**
   * Unsupported series format
   */
  void addSeries(Series2D<std::string, double>* series);

  /**
   * Unsupported series format
   */
  void addSeries(Series2D<std::string, std::string>* series);

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

  struct Line {
    std::vector<std::pair<double, double>> points;
    std::string line_style;
    double line_width;
    std::string point_style;
    double point_size;
    std::string color;
    bool smooth;
  };

  Canvas* canvas_;
  NumericalDomain* x_domain_;
  NumericalDomain* y_domain_;
  int num_series_;
  std::vector<Line> lines_;
  mutable std::unique_ptr<NumericalDomain> x_domain_auto_;
  mutable std::unique_ptr<NumericalDomain> y_domain_auto_;
};

}
}
#endif

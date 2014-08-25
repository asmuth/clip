/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
      Domain* x_domain = nullptr,
      Domain* y_domain = nullptr);

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
   * Add a (x: string, y: double) series. This will draw one connected line
   * through all points in the series
   *
   * @param series the series to add. does not transfer ownership
   * @param line_width the line widht
   * @param smooth smooth this line?
   * @param line_style the line style ({solid,dashed})
   */
  void addSeries(
      Series2D<std::string, double>* series,
      const std::string& line_style = kDefaultLineStyle,
      double line_width = kDefaultLineWidth,
      const std::string& point_style = kDefaultPointStyle,
      double point_size = kDefaultPointSize,
      bool smooth = false);

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

  Domain* getXDomain() const;
  Domain* getYDomain() const;

  void render(
      RenderTarget* target,
      int width,
      int height,
      std::tuple<int, int, int, int>* padding) const override;

  template <typename T>
  struct Line {
    std::vector<std::pair<T, double>> points;
    std::string line_style;
    double line_width;
    std::string point_style;
    double point_size;
    std::string color;
    bool smooth;
  };

  template <typename T>
  void drawLine(
      RenderTarget* target,
      T line,
      std::vector<std::pair<double, double>>& coords);

  Canvas* canvas_;
  Domain* x_domain_;
  Domain* y_domain_;
  int num_series_;
  std::vector<Line<double>> lines_;
  std::vector<Line<std::string>> categorical_lines_;
  mutable std::unique_ptr<Domain> x_domain_auto_;
  mutable std::unique_ptr<Domain> y_domain_auto_;
};

}
}
#endif

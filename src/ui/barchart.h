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
#include <fnordmetric/base/series.h>
#include <fnordmetric/ui/axisdefinition.h>
#include <fnordmetric/ui/domain.h>
#include <fnordmetric/ui/drawable.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace ui {
class Canvas;
class Domain;

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
      kBarChartOrientation orientation = O_HORIZONTAL,
      bool stacked = false,
      NumericalDomain* y_domain = nullptr);

  /**
   * Add a (x: string, y: double) series. This will draw one bar for each point
   * in the series where x is the label of the bar and y is the height of the
   * bar
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series2D<std::string, double>* series);

  /**
   * Add a (x: double, y: double) series. This will draw one bar for each point
   * in the series where x is the label of the bar and y is the height of the
   * bar
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series2D<double, double>* series);

  /**
   * Add a (x: string, y: double, z: double) series. This will draw one bar for
   * each point in the series where x is the label of the bar, y is the lower
   * bound of the bar and z is the upper bound of the bar.
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series3D<std::string, double, double>* series);

  /**
   * Add a (x: double, y: double, z: double) series. This will draw one bar for
   * each point in the series where x is the label of the bar, y is the lower
   * bound of the bar and z is the upper bound of the bar.
   *
   * @param series the series to add. does not transfer ownership
   */
  void addSeries(Series3D<double, double, double>* series);

  void addSeries(Series* series) {
    RAISE(util::RuntimeException, "unsupported series type for barchart");
  }

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

  NumericalDomain* getValueDomain() const;
  NumericalDomain* newValueDomain() const;
  AxisDefinition* newLabelAxis(AxisDefinition::kPosition position) const;

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
    std::string x;
    std::vector<std::pair<double, double>> ys;
  };

  Canvas* canvas_;
  kBarChartOrientation orientation_;
  bool stacked_;
  NumericalDomain* y_domain_;
  mutable std::unique_ptr<NumericalDomain> y_domain_auto_;
  std::vector<BarData> data_;
  int num_series_;
  std::vector<std::string> series_colors_;
};

}
}
#endif

/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include "plot_layout.h"
#include "../graphics/rendertarget.h"
#include "axisdefinition.h"
#include "continuousdomain.h"
#include "domain.h"
#include "drawable.h"
#include "colorpalette.h"

namespace signaltk {
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
class LineChart {
public:
  using TX = double;
  using TY = double;

  static const constexpr int kLabelPadding = 8;
  static char kDefaultLineStyle[];
  static char kDefaultLineWidth[];
  static char kDefaultPointStyle[];
  static char kDefaultPointSize[];

  /**
   * Create a new line chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  LineChart(PlotLayout* canvas);

  /**
   * Create a new line chart
   *
   * @param canvas the canvas to draw this chart on. does not transfer ownership
   * @param x_domain the x domain. does not transfer ownership
   * @param y_domain the y domain. does not transfer ownership
   */
  LineChart(PlotLayout* canvas, AnyDomain* x_domain, AnyDomain* y_domain);

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
   * Get the {x,y} domain of this chart. Will raise an exception if z domain
   * is requested.
   *
   * The returned pointer is owned by the chart object and must not be freed
   * by the caller!
   *
   * @param dimension the dimension (x,y)
   */
  AnyDomain* getDomain(AnyDomain::kDimension dimension);

  void setLabels(bool show_labels);

  void render(
      RenderTarget* target,
      Viewport* viewport) const;

protected:
  DomainProvider x_domain_;
  DomainProvider y_domain_;
  std::vector<Series2D<TX, TY>*> series_;
  ColorPalette color_palette_;
  bool show_labels_;
};

}
}

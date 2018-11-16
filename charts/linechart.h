/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <stdlib.h>
#include <graphics/layer.h>
#include <graphics/viewport.h>
#include "series.h"
#include "continuousdomain.h"
#include "domain.h"
#include "domainprovider.h"
#include "colorpalette.h"

namespace plotfx {
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
  LineChart(AnyDomain* x_domain, AnyDomain* y_domain);

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
      Layer* target,
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

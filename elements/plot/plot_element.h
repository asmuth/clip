/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include "signaltk.h"
#include "elements/element.h"
#include "elements/element_spec.h"
#include "utils/return_code.h"
#include "plot_axis.h"
#include "plot_domain.h"

namespace signaltk {

struct PlotConfig {
  PlotConfig();
  PlotDomain x_domain;
  std::optional<double> x_min;
  std::optional<double> x_max;
  PlotDomain y_domain;
  std::optional<double> y_min;
  std::optional<double> y_max;
  AxisDefinition axis_top;
  AxisDefinition axis_right;
  AxisDefinition axis_bottom;
  AxisDefinition axis_left;
};

ReturnCode renderPlot(const PlotConfig& config, Layer* frame);

ReturnCode configurePlot(const PropertyList& plist, PlotConfig* config);

class PlotElement : public Element {
public:

  static ReturnCode configure(
      const PropertyList& plist,
      std::unique_ptr<Element>* elem);

  explicit PlotElement(const PlotConfig& config);

  ReturnCode renderTo(Layer* frame) const override;

protected:
  PlotConfig config;
};

} // namespace signaltk


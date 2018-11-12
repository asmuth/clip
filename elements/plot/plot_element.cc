/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "elements/plot/plot_element.h"
#include "core/config_helpers.h"

namespace signaltk {

PlotConfig::PlotConfig() :
    x_domain(PlotDomain::LINEAR),
    y_domain(PlotDomain::LINEAR) {}

ReturnCode renderPlot(const PlotConfig& config, Layer* frame) {
  // render axes
  if (auto rc = renderAxis(config.axis_top, AxisPosition::TOP, frame); rc) {
    return rc;
  }

  if (auto rc = renderAxis(config.axis_right, AxisPosition::RIGHT, frame); rc) {
    return rc;
  }

  if (auto rc = renderAxis(config.axis_bottom, AxisPosition::BOTTOM, frame); rc) {
    return rc;
  }

  if (auto rc = renderAxis(config.axis_left, AxisPosition::LEFT, frame); rc) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode configurePlotAxisTop(const plist::Property& prop, PlotConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_top.mode);
}

ReturnCode configurePlotAxisRight(const plist::Property& prop, PlotConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_right.mode);
}

ReturnCode configurePlotAxisBottom(const plist::Property& prop, PlotConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_bottom.mode);
}

ReturnCode configurePlotAxisLeft(const plist::Property& prop, PlotConfig* config) {
  if (prop.size() != 1) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop[0], &config->axis_left.mode);
}

ReturnCode configurePlot(const PropertyList& plist, PlotConfig* config) {
  static const PropertyDefinitions<PlotConfig> pdefs = {
    {"axis-top", &configurePlotAxisTop},
    {"axis-right", &configurePlotAxisRight},
    {"axis-bottom", &configurePlotAxisBottom},
    {"axis-left", &configurePlotAxisLeft},
  };

  if (auto rc = configureProperties(plist, pdefs, config); !rc.isSuccess()) {
    return rc;
  }

  return ReturnCode::success();
}

ReturnCode PlotElement::configure(
    const PropertyList& plist,
    std::unique_ptr<Element>* elem) {
  PlotConfig config;
  if (auto rc = configurePlot(plist, &config); !rc.isSuccess()) {
    return rc;
  }

  *elem = std::make_unique<PlotElement>(config);
  return ReturnCode::success();
}

PlotElement::PlotElement(const PlotConfig& c) : config(c) {}

ReturnCode PlotElement::renderTo(Layer* frame) const {
  return renderPlot(config, frame);
}

} // namespace signaltk


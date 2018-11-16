/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "charts/plot_element.h"
#include "common/config_helpers.h"

namespace plotfx {

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

} // namespace plotfx


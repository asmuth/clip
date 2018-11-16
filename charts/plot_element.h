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
#pragma once
#include <memory>
#include "plotfx.h"
#include "core/plist/plist.h"
#include "elements/element.h"
#include "utils/return_code.h"
#include "plot_axis.h"
#include "plot_domain.h"

namespace plotfx {

struct PlotConfig {
  PlotConfig();
  PlotDomain x_domain;
  //std::optional<double> x_min;
  //std::optional<double> x_max;
  PlotDomain y_domain;
  //std::optional<double> y_min;
  //std::optional<double> y_max;
  AxisDefinition axis_top;
  AxisDefinition axis_right;
  AxisDefinition axis_bottom;
  AxisDefinition axis_left;
};

ReturnCode renderPlot(const PlotConfig& config, Layer* frame);

ReturnCode configurePlot(const plist::PropertyList& plist, PlotConfig* config);

class PlotElement : public Element {
public:

  static ReturnCode configure(
      const plist::PropertyList& plist,
      std::unique_ptr<Element>* elem);

  explicit PlotElement(const PlotConfig& config);

  ReturnCode renderTo(Layer* frame) const override;

protected:
  PlotConfig config;
};

} // namespace plotfx


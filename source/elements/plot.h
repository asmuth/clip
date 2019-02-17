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
#include "plist/plist.h"
#include "core/environment.h"
#include "element.h"
#include "core/layout.h"

namespace plotfx {
namespace plot {

struct PlotBorderConfig {
  Color color;
  Measure width;
};

struct PlotConfig {
  std::vector<ElementRef> children;
  FontInfo font;
  Measure font_size;
  ColorScheme color_scheme;
  std::optional<Color> background;
  Color text_color;
  Color border_color;
  DomainConfig scale_x;
  DomainConfig scale_y;
  ScaleLayoutFn scale_layout_x;
  ScaleLayoutFn scale_layout_y;
  std::array<Measure, 4> margins;
  std::array<PlotBorderConfig, 4> borders;
  LayoutSettings layout;
};

ReturnCode draw(
    const PlotConfig& config,
    const LayoutInfo& layout,
    Layer* layer);

ReturnCode reflow(
    const PlotConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    PlotConfig* config);

} // namespace plot
} // namespace plotfx


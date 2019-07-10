/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
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
#include <utility>
#include <string>
#include <vector>
#include <sexpr.h>
#include <graphics/layer.h>
#include <utils/return_code.h>
#include <source/core/format.h>
#include <source/dimension.h>
#include "core/format.h"
#include "core/environment.h"
#include "core/layout.h"

namespace plotfx {
struct DomainConfig;
struct LayoutState;
struct AxisDefinition;

enum class AxisPosition {
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
  CENTER_HORIZ,
  CENTER_VERT
};

enum class AxisLabelPosition {
  LEFT,
  RIGHT,
  TOP,
  BOTTOM,
  INSIDE,
  OUTSIDE,
};

using AxisLabelPlacement = std::function<ReturnCode (
    const DomainConfig& domain,
    AxisDefinition*)>;

struct AxisDefinition {
  AxisDefinition();
  AxisPosition position;
  DomainConfig scale;
  ScaleLayoutFn scale_layout;
  std::string title;
  std::vector<std::string> label_override;
  AxisLabelPosition tick_position;
  AxisLabelPosition label_position;
  Formatter label_formatter;
  Color text_color;
  Color border_color;
  FontInfo font;
  Measure label_padding;
  Measure label_font_size;
  Measure tick_length;
  LayoutSettings layout;
};

namespace axis {

ReturnCode draw(
    const AxisDefinition& config,
    const LayoutInfo& layout,
    Layer* layer);

ReturnCode reflow(
    const AxisDefinition& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height);

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    AxisDefinition* config);


} // namespace axis
} // namespace plotfx


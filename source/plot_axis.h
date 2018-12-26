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
#include <plist/plist.h>
#include <graphics/layer.h>
#include <graphics/viewport.h>
#include <utils/return_code.h>
#include <source/format.h>
#include <source/dimension.h>

namespace plotfx {
struct DomainConfig;
struct Rectangle;
struct AxisDefinition;

enum class AxisPosition {
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
  CENTER_HORIZ,
  CENTER_VERT
};

enum class AxisMode {
  OFF,
  AUTO,
  MANUAL
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
  AxisMode mode;
  std::string scale;
  std::string title;
  std::vector<double> ticks;
  std::vector<std::pair<double, std::string>> labels;
  AxisLabelPosition tick_position;
  AxisLabelPosition label_position;
  AxisLabelPlacement label_placement;
  Formatter label_formatter;
  Color text_color;
  Color border_color;
  FontInfo font;
  Measure label_padding;
  Measure label_font_size;
  Measure tick_length;
};

ReturnCode parseAxisMode(
    const std::string& str,
    AxisMode* value);

ReturnCode parseAxisModeProp(
    const plist::Property& prop,
    AxisMode* value);

Status renderAxis(
    const AxisDefinition& axis,
    const Rectangle& clip,
    AxisPosition axis_position,
    Layer* frame);

ReturnCode axis_layout(
    const Rectangle& parent,
    const AxisDefinition& axis_top,
    const AxisDefinition& axis_right,
    const AxisDefinition& axis_bottom,
    const AxisDefinition& axis_left,
    const Layer& layer,
    Rectangle* bbox);

ReturnCode axis_draw_all(
    const Rectangle& clip,
    const AxisDefinition& axis_top,
    const AxisDefinition& axis_right,
    const AxisDefinition& axis_bottom,
    const AxisDefinition& axis_left,
    Layer* layer);

ReturnCode axis_place_labels_default(
    const DomainConfig& domain,
    AxisDefinition* axis);

ReturnCode axis_place_labels_geom(
    const DomainConfig& domain,
    AxisDefinition* axis);

ReturnCode axis_place_labels_categorical(
    const DomainConfig& domain,
    AxisDefinition* axis);

ReturnCode axis_configure_label_placement(
    const plist::Property& prop,
    AxisLabelPlacement* placement);

ReturnCode axis_configure(
    AxisDefinition* axis,
    const AxisLabelPosition& axis_position,
    const DomainConfig& domain);

ReturnCode axis_resolve(
    const DomainMap& scales,
    AxisDefinition* axis_top,
    AxisDefinition* axis_right,
    AxisDefinition* axis_bottom,
    AxisDefinition* axis_left);

} // namespace plotfx


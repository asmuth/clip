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
#include "plot_axis.h"
#include <assert.h>
#include <iostream>
#include <common/config_helpers.h>
#include <graphics/text.h>
#include <graphics/brush.h>

namespace plotfx {

AxisDefinition::AxisDefinition() :
    mode(AxisMode::OFF),
    label_placement(AxisLabelPlacement::OFF),
    padding_rem(4.0f),
    label_padding_rem(1.0f),
    label_font_size_rem(1.0f),
    tick_length_rem(0.4f) {}

ReturnCode parseAxisMode(
    const std::string& str,
    AxisMode* value) {
  static const EnumDefinitions<AxisMode> defs = {
    { "auto", AxisMode::AUTO },
    { "off", AxisMode::OFF },
    { "manual", AxisMode::MANUAL },
  };

  return parseEnum(defs, str, value);
}

static Status renderAxisVertical(
    const AxisDefinition& axis_config,
    double x,
    double y0,
    double y1,
    Layer* target) {
  /* draw axis line */ 
  {
    StrokeStyle style;
    strokeLine(target, x, y0, x, y1, style);
  }

  double label_placement = 0;
  switch (axis_config.label_placement) {
    case AxisLabelPlacement::RIGHT:
      label_placement = 1;
      break;
    case AxisLabelPlacement::LEFT:
      label_placement = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.ticks) {
    auto y = y0 + (y1 - y0) * tick;
    StrokeStyle style;
    strokeLine(
        target,
        x,
        y,
        x + from_rem(*target, axis_config.tick_length_rem) * label_placement,
        y,
        style);
  }

  /* draw labels */
  auto label_padding = from_rem(*target, axis_config.label_padding_rem);
  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
    auto sy = y0 + (y1 - y0) * tick;
    auto sx = x + label_padding * label_placement;

    TextStyle style;
    style.halign = label_placement > 0 ? TextHAlign::LEFT : TextHAlign::RIGHT;
    style.valign = TextVAlign::MIDDLE;
    if (auto rc = drawText(label_text, sx, sy, style, target); rc != OK) {
      return rc;
    }
  }

  return OK;
}

static Status renderAxisHorizontal(
    const AxisDefinition& axis_config,
    double y,
    double x0,
    double x1,
    Layer* target) {
  /* draw axis line */ 
  {
    StrokeStyle style;
    strokeLine(target, x0, y, x1, y, style);
  }

  double label_placement = 0;
  switch (axis_config.label_placement) {
    case AxisLabelPlacement::BOTTOM:
      label_placement = 1;
      break;
    case AxisLabelPlacement::TOP:
      label_placement = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.ticks) {
    auto x = x0 + (x1 - x0) * tick;
    StrokeStyle style;
    strokeLine(
        target,
        x,
        y,
        x,
        y + from_rem(*target, axis_config.tick_length_rem) * label_placement,
        style);
  }

  /* draw labels */
  auto label_padding = from_rem(*target, axis_config.label_padding_rem);
  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
    auto sx = x0 + (x1 - x0) * tick;
    auto sy = y + label_padding * label_placement;

    TextStyle style;
    style.halign = TextHAlign::CENTER;
    style.valign = label_placement > 0 ? TextVAlign::TOP : TextVAlign::BOTTOM;
    if (auto rc = drawText(label_text, sx, sy, style, target); rc) {
      return rc;
    }
  }

  return OK;
}

Status renderAxis(
    const AxisDefinition& axis,
    AxisPosition axis_position,
    Layer* frame) {
  switch (axis.mode) {
    case AxisMode::OFF:
      return OK;
    default:
      break;
  };

  auto padding = from_rem(*frame, axis.padding_rem);

  Status rc;
  switch (axis_position) {
    case AxisPosition::LEFT:
      rc = renderAxisVertical(
          axis,
          padding,
          padding,
          frame->height - padding,
          frame);
      break;
    case AxisPosition::RIGHT:
      rc = renderAxisVertical(
          axis,
          frame->width - padding,
          padding,
          frame->height - padding,
          frame);
      break;
    case AxisPosition::TOP:
      rc = renderAxisHorizontal(
          axis,
          padding,
          padding,
          frame->width - padding,
          frame);
      break;
    case AxisPosition::BOTTOM:
      rc = renderAxisHorizontal(
          axis,
          frame->height - padding,
          padding,
          frame->width - padding,
          frame);
      break;
  }

  return rc;
}

ReturnCode axis_expand_linear_geom(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  uint32_t num_ticks = 6;

  for (size_t i = 0; i < num_ticks; ++i) {
    auto o = (1.0f / (num_ticks - 1)) * i;
    axis->ticks.emplace_back(o);
    axis->labels.emplace_back(o, std::to_string(o)); // FIXME
  }

  return OK;
}

ReturnCode axis_expand_auto(
    const AxisDefinition& in,
    const DomainConfig& domain,
    AxisDefinition* out) {
  *out = in;
  return axis_expand_linear_geom(domain, out); // FIXME
}


}

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
#include <common/domain.h>
#include <graphics/text.h>
#include <graphics/layout.h>
#include <graphics/brush.h>

namespace plotfx {

AxisDefinition::AxisDefinition() :
    mode(AxisMode::AUTO),
    label_position(AxisLabelPosition::OUTSIDE),
    label_formatter(format_decimal_fixed(1)),
    label_padding({Unit::REM, 0.8f}),
    label_font_size({Unit::REM, 1.0f}),
    tick_length({Unit::REM, 0.4f}) {}

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

ReturnCode parseAxisModeProp(const plist::Property& prop, AxisMode* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop, value);
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
    style.colour = axis_config.border_colour;
    strokeLine(target, {x, y0}, {x, y1}, style);
  }

  double label_position = 0;
  switch (axis_config.label_position) {
    case AxisLabelPosition::RIGHT:
      label_position = 1;
      break;
    case AxisLabelPosition::LEFT:
      label_position = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.ticks) {
    auto y = y0 + (y1 - y0) * (1.0 - tick);
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    strokeLine(
        target,
        {x, y},
        {x + to_px(target->measures, axis_config.tick_length).value * label_position, y},
        style);
  }

  /* draw labels */
  auto label_padding = to_px(target->measures, axis_config.label_padding).value;
  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;

    auto p = Point {
     .x = x + label_padding * label_position,
     .y = y0 + (y1 - y0) * (1.0 - tick),
    };

    TextStyle style;
    style.font = axis_config.font;
    style.colour = axis_config.text_colour;
    auto ax = label_position > 0 ? HAlign::LEFT : HAlign::RIGHT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); rc != OK) {
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
    style.colour = axis_config.border_colour;
    strokeLine(target, {x0, y}, {x1, y}, style);
  }

  double label_position = 0;
  switch (axis_config.label_position) {
    case AxisLabelPosition::BOTTOM:
      label_position = 1;
      break;
    case AxisLabelPosition::TOP:
      label_position = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.ticks) {
    auto x = x0 + (x1 - x0) * tick;
    StrokeStyle style;
    style.colour = axis_config.border_colour;
    strokeLine(
        target,
        {x, y},
        {x, y + to_px(target->measures, axis_config.tick_length).value * label_position},
        style);
  }

  /* draw labels */
  auto label_padding = to_px(target->measures, axis_config.label_padding).value;
  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
    auto p = Point {
      .x = x0 + (x1 - x0) * tick,
      .y = y + label_padding * label_position,
    };

    TextStyle style;
    style.font = axis_config.font;
    style.colour = axis_config.text_colour;
    auto ax = HAlign::CENTER;
    auto ay = label_position > 0 ? VAlign::TOP : VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); rc) {
      return rc;
    }
  }

  return OK;
}

Status renderAxis(
    const AxisDefinition& axis,
    const Rectangle& clip,
    AxisPosition axis_position,
    Layer* frame) {
  switch (axis.mode) {
    case AxisMode::OFF:
      return OK;
    default:
      break;
  };

  Status rc;
  switch (axis_position) {
    case AxisPosition::LEFT:
      rc = renderAxisVertical(
          axis,
          clip.x,
          clip.y,
          clip.y + clip.h,
          frame);
      break;
    case AxisPosition::RIGHT:
      rc = renderAxisVertical(
          axis,
          clip.x + clip.w,
          clip.y,
          clip.y + clip.h,
          frame);
      break;
    case AxisPosition::TOP:
      rc = renderAxisHorizontal(
          axis,
          clip.y,
          clip.x,
          clip.x + clip.w,
          frame);
      break;
    case AxisPosition::BOTTOM:
      rc = renderAxisHorizontal(
          axis,
          clip.y + clip.h,
          clip.x,
          clip.x + clip.w,
          frame);
      break;
  }

  return rc;
}

ReturnCode axis_expand_linear_geom(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  uint32_t num_ticks = 8; // FIXME make configurable
  double min = domain.min.value_or(0.0f);
  double max = domain.max.value_or(0.0f);

  axis->ticks.clear();
  axis->labels.clear();

  for (size_t i = 0; i < num_ticks; ++i) {
    axis->ticks.emplace_back((1.0f / (num_ticks - 1)) * i);
  }

  auto tick_values = domain_untranslate(domain, axis->ticks);
  for (size_t i = 0; i < num_ticks; ++i) {
    axis->labels.emplace_back(
        axis->ticks[i],
        axis->label_formatter.format_value(tick_values[i]));
  }

  return OK;
}

ReturnCode axis_expand_auto(
    const AxisDefinition& in,
    const AxisPosition& pos,
    const DomainConfig& domain,
    AxisDefinition* out) {
  *out = in;

  switch (out->label_position) {
    case AxisLabelPosition::OUTSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->label_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::RIGHT:
          out->label_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          out->label_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::LEFT:
          out->label_position = AxisLabelPosition::LEFT;
          break;
      }
      break;
    case AxisLabelPosition::INSIDE:
      switch (pos) {
        case AxisPosition::TOP:
          out->label_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::RIGHT:
          out->label_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::BOTTOM:
          out->label_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::LEFT:
          out->label_position = AxisLabelPosition::RIGHT;
          break;
      }
      break;
  };

  return axis_expand_linear_geom(domain, out); // FIXME
}

ReturnCode axis_draw_all(
    const Rectangle& clip,
    const DomainConfig& domain_x,
    const DomainConfig& domain_y,
    const AxisDefinition& axis_top_,
    const AxisDefinition& axis_right_,
    const AxisDefinition& axis_bottom_,
    const AxisDefinition& axis_left_,
    Layer* layer) {
  AxisDefinition axis_top;
  if (auto rc = axis_expand_auto(axis_top_, AxisPosition::TOP, domain_x, &axis_top); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_top, clip, AxisPosition::TOP, layer); rc) {
    return rc;
  }

  AxisDefinition axis_right;
  if (auto rc = axis_expand_auto(axis_right_, AxisPosition::RIGHT, domain_y, &axis_right); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_right, clip, AxisPosition::RIGHT, layer); rc) {
    return rc;
  }

  AxisDefinition axis_bottom;
  if (auto rc = axis_expand_auto(axis_bottom_, AxisPosition::BOTTOM, domain_x, &axis_bottom); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_bottom, clip, AxisPosition::BOTTOM, layer); rc) {
    return rc;
  }

  AxisDefinition axis_left;
  if (auto rc = axis_expand_auto(axis_left_, AxisPosition::LEFT, domain_y, &axis_left); !rc) {
    return rc;
  }

  if (auto rc = renderAxis(axis_left, clip, AxisPosition::LEFT, layer); rc) {
    return rc;
  }

  return OK;
}

} // namespace plotfx


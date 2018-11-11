/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "plot_axis.h"
#include <assert.h>
#include <elements/plot/plot_element.h>
#include <elements/element_config_helpers.h>
#include <graphics/text.h>
#include <graphics/brush.h>

namespace signaltk {

AxisDefinition::AxisDefinition() :
    mode(AxisMode::OFF),
    label_placement(AxisLabelPlacement::OFF),
    label_padding_horiz_rem(kDefaultLabelPaddingHorizREM),
    label_padding_vert_rem(kDefaultLabelPaddingVertREM),
    tick_length_rem(kDefaultTickLengthREM) {}

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
  auto label_padding = from_rem(*target, axis_config.label_padding_horiz_rem);
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
  auto label_padding = from_rem(*target, axis_config.label_padding_vert_rem);
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

  int padding = 80;

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


}

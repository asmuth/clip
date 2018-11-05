/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "axes.h"
#include <graphics/text.h>
#include <graphics/brush.h>

namespace signaltk {
namespace chart {

AxisDefinition::AxisDefinition() :
    enabled_(false),
    has_ticks_(false),
    has_labels_(false),
    label_padding_horiz_rem(kDefaultLabelPaddingHorizREM),
    label_padding_vert_rem(kDefaultLabelPaddingVertREM),
    tick_length_rem(kDefaultTickLengthREM) {}

void AxisDefinition::addTick(double tick_position) {
  ticks_.push_back(tick_position);
}

const std::vector<double> AxisDefinition::getTicks() const {
  return ticks_;
}

void AxisDefinition::addLabel(
    double label_position,
    const std::string& label_text) {
  has_labels_ = true;
  labels_.emplace_back(label_position, label_text);
}

void AxisDefinition::removeLabels() {
  labels_.clear();
}

const std::vector<std::pair<double, std::string>> AxisDefinition::getLabels()
    const {
  return labels_;
}

bool AxisDefinition::hasLabels() const {
  return has_labels_;
}

void AxisDefinition::setTitle(const std::string& title) {
  title_ = title;
}

const std::string& AxisDefinition::getTitle() {
  return title_;
}

bool AxisDefinition::hasTitle() const {
  return title_.length() > 0;
}

Status renderAxisVertical(
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
    case AxisDefinition::LABELS_RIGHT:
      label_placement = 1;
      break;
    case AxisDefinition::LABELS_LEFT:
      label_placement = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.getTicks()) {
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
  for (const auto& label : axis_config.getLabels()) {
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

Status renderAxisHorizontal(
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
    case AxisDefinition::LABELS_BOTTOM:
      label_placement = 1;
      break;
    case AxisDefinition::LABELS_TOP:
      label_placement = -1;
      break;
    default:
      break;
  }

  /* draw ticks */
  for (const auto& tick : axis_config.getTicks()) {
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
  for (const auto& label : axis_config.getLabels()) {
    auto [ tick, label_text ] = label;
    auto sx = x0 + (x1 - x0) * tick;
    auto sy = y + label_padding * label_placement;

    TextStyle style;
    style.halign = TextHAlign::CENTER;
    style.valign = label_placement > 0 ? TextVAlign::TOP : TextVAlign::BOTTOM;
    if (auto rc = drawText(label_text, sx, sy, style, target); rc != OK) {
      return rc;
    }
  }

  return OK;
}

}
}

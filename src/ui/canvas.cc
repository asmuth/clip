/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "canvas.h"
#include "rendertarget.h"
#include "seriesdefinition.h"
#include "ui/domain.h"
#include "ui/rendertarget.h"
#include "ui/svgtarget.h"

namespace fnordmetric {
namespace ui {

Canvas::Canvas() :
    width_(1024),
    height_(500) {}

void Canvas::render(RenderTarget* target) const {
  std::tuple<int, int, int, int> padding;

  target->beginChart(width_, height_, "chart bar horizontal");
  renderAxes(target, &padding);
  target->finishChart();
}

void Canvas::renderAxes(
    RenderTarget* target,
    std::tuple<int, int, int, int>* padding) const {
  std::vector<std::pair<int, AxisDefinition*>> top;
  std::vector<std::pair<int, AxisDefinition*>> right;
  std::vector<std::pair<int, AxisDefinition*>> bottom;
  std::vector<std::pair<int, AxisDefinition*>> left;

  for (const auto& axis : axes_) {
    switch (axis->getPosition()) {

      case AxisDefinition::TOP: {
        top.emplace_back(std::get<0>(*padding), axis.get());
        std::get<0>(*padding) += kAxisLabelHeight;
        std::get<0>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::RIGHT: {
        right.emplace_back(std::get<1>(*padding), axis.get());
        std::get<1>(*padding) += kAxisLabelWidth;
        std::get<1>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::BOTTOM: {
        bottom.emplace_back(std::get<2>(*padding), axis.get());
        std::get<2>(*padding) += kAxisLabelHeight;
        std::get<2>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::LEFT: {
        left.emplace_back(std::get<3>(*padding), axis.get());
        std::get<3>(*padding) += kAxisLabelWidth;
        std::get<3>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

    }
  }

  for (const auto& placement : left) {
    renderLeftAxis(target, placement.second, padding, placement.first);
  }

/*
  if (padding_top_ < kAxisLabelHeight * 0.5f) {
    padding_top_ = kAxisLabelHeight * 0.5f;
  }

  if (padding_right_ < kAxisLabelWidth * 0.5f) {
    padding_right_ = kAxisLabelWidth * 0.5f;
  }

  if (padding_bottom_ < kAxisLabelHeight * 0.5f) {
    padding_bottom_ = kAxisLabelHeight * 0.5f;
  }

  if (padding_left_ < kAxisLabelWidth * 0.5f) {
    padding_left_ = kAxisLabelWidth * 0.5f;
  }
*/

}

void Canvas::renderLeftAxis(
    RenderTarget* target,
    AxisDefinition* axis,
    std::tuple<int, int, int, int>* padding,
    int left) const {
  int padding_top = std::get<0>(*padding);
  int inner_height = height_ - std::get<2>(*padding) - padding_top;

  target->beginGroup("axis left");

  /* draw title */
  if (axis->hasTitle()) {
    target->drawText(
        axis->getTitle(),
        left,
        padding_top + inner_height * 0.5f,
        "middle",
        "text-before-edge",
        "title",
        270);

    left += kAxisTitleLength;
  }

  /* draw labels */
  for (const auto& label : axis->getLabels()) {
    auto tick_y = padding_top + inner_height * label.first;

    target->drawText(
        label.second,
        left - (kTickLength * 2),
        tick_y,
        "end",
        "middle",
        "label");
  }

  /* draw ticks */
  /* for (const auto& tick : ticks) {
    auto tick_y = padding_top_ + inner_height_ * tick;

    target->drawLine(
        padding_left_,
        tick_y,
        padding_left_ - kTickLength,
        tick_y,
        "tick");
  }
  */

  /* draw stroke */
  target->drawLine(
      left,
      padding_top,
      left,
      padding_top + inner_height,
      "stroke");

  target->finishGroup();
}

std::string Canvas::renderSVG() const {
  SVGTarget target;
  render(&target);
  return "fnord";
}

AxisDefinition* Canvas::addAxis(AxisDefinition::kPosition axis_position) {
  auto axis = new AxisDefinition(axis_position);
  axes_.emplace_back(axis);
  return axis;
}

}
}

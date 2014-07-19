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

  target->beginChart(width_, height_, "fnord");
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
  target->beginGroup("axis left");

  left += kAxisTitleLength;

  /* draw stroke */
  target->drawLine(
      left,
      std::get<0>(*padding),
      left,
      height_ - std::get<2>(*padding),
      "stroke");

  /* draw title */
  /*if (axis_title_[LEFT].size() > 0) {
    target->drawText(
        axis_title_[LEFT],
        padding_left_ - kAxisLabelWidth - kAxisTitleLength,
        padding_top_ + inner_height_ * 0.5f,
        "middle",
        "text-before-edge",
        "title",
        270);
  }*/

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

  /* draw labels */
  /*
  for (const auto& label : labels) {
    auto tick_y = padding_top_ + inner_height_ * label.first;

    target->drawText(
        label.second,
        padding_left_ - (kTickLength * 2),
        tick_y,
        "end",
        "middle",
        "label");
  }
  */

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

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
  for (const auto& drawable : drawables_) {
    drawable->render(target, width_, height_, &padding);
  }
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
        std::get<0>(*padding) += kAxisPadding;
        std::get<0>(*padding) += axis->hasLabels() ? kAxisLabelHeight : 0;
        std::get<0>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::RIGHT: {
        right.emplace_back(std::get<1>(*padding), axis.get());
        std::get<1>(*padding) += kAxisPadding;
        std::get<1>(*padding) += axis->hasLabels() ? kAxisLabelWidth : 0;
        std::get<1>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::BOTTOM: {
        bottom.emplace_back(std::get<2>(*padding), axis.get());
        std::get<2>(*padding) += kAxisPadding;
        std::get<2>(*padding) += axis->hasLabels() ? kAxisLabelHeight : 0;
        std::get<2>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::LEFT: {
        left.emplace_back(std::get<3>(*padding), axis.get());
        std::get<3>(*padding) += kAxisPadding;
        std::get<3>(*padding) += axis->hasLabels() ? kAxisLabelWidth : 0;
        std::get<3>(*padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

    }
  }

  if (std::get<0>(*padding) < kAxisLabelHeight * 0.5f) {
    std::get<0>(*padding) += kAxisLabelHeight * 0.5f;
  }

  if (std::get<1>(*padding) < kAxisLabelWidth * 0.5f) {
    std::get<1>(*padding) += kAxisLabelWidth * 0.5f;
  }

  if (std::get<2>(*padding) < kAxisLabelHeight * 0.5f) {
    std::get<2>(*padding) += kAxisLabelHeight * 0.5f;
  }

  if (std::get<3>(*padding) < kAxisLabelWidth * 0.5f) {
    std::get<3>(*padding) += kAxisLabelWidth * 0.5f;
  }

  for (const auto& placement : top) {
    renderTopAxis(target, placement.second, padding, placement.first);
  }

  for (const auto& placement : right) {
    renderRightAxis(target, placement.second, padding, placement.first);
  }

  for (const auto& placement : bottom) {
    renderBottomAxis(target, placement.second, padding, placement.first);
  }

  for (const auto& placement : left) {
    renderLeftAxis(target, placement.second, padding, placement.first);
  }
}

void Canvas::renderTopAxis(
    RenderTarget* target,
    AxisDefinition* axis,
    std::tuple<int, int, int, int>* padding,
    int top) const {
  int padding_left = std::get<3>(*padding);
  int inner_width = width_ - std::get<1>(*padding) - padding_left;

  top += kAxisPadding;
  target->beginGroup("axis bottom");

  /* draw title */
  if (axis->hasTitle()) {
    target->drawText(
        axis->getTitle(),
        padding_left + inner_width* 0.5f,
        top,
        "middle",
        "text-before-edge",
        "title");

    top += kAxisTitleLength;
  }

  /* draw labels */
  if (axis->hasLabels()) {
    top += kAxisLabelHeight; // FIXPAUL: calculate label width?

    for (const auto& label : axis->getLabels()) {
      auto tick_x = padding_left + inner_width * label.first;

      target->drawText(
          label.second,
          tick_x,
          top - (kTickLength * 2),
          "middle",
          "text-after-edge",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_x = padding_left + inner_width * tick;

    target->drawLine(
        tick_x,
        top,
        tick_x,
        top + kTickLength,
        "tick");
  }

  /* draw stroke */
  target->drawLine(
      padding_left,
      top,
      padding_left + inner_width,
      top,
      "stroke");

  target->finishGroup();
}


void Canvas::renderRightAxis(
    RenderTarget* target,
    AxisDefinition* axis,
    std::tuple<int, int, int, int>* padding,
    int right) const {
  int padding_top = std::get<0>(*padding);
  int inner_height = height_ - std::get<2>(*padding) - padding_top;

  right += kAxisPadding;
  target->beginGroup("axis right");

  /* draw title */
  if (axis->hasTitle()) {
    right += kAxisTitleLength;
    target->drawText(
        axis->getTitle(),
        width_ - right,
        padding_top + inner_height * 0.5f,
        "middle",
        "text-before-edge",
        "title",
        270);
  }

  /* draw labels */
  if (axis->hasLabels()) {
    right += kAxisLabelWidth; // FIXPAUL: calculate label width?

    for (const auto& label : axis->getLabels()) {
      auto tick_y = padding_top + inner_height * (1.0 - label.first);

      target->drawText(
          label.second,
          width_ - right + (kTickLength * 2),
          tick_y,
          "start",
          "middle",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_y = padding_top + inner_height * (1.0 - tick);

    target->drawLine(
        width_ - right,
        tick_y,
        width_ - right - kTickLength,
        tick_y,
        "tick");
  }

  /* draw stroke */
  target->drawLine(
      width_ - right,
      padding_top,
      width_ - right,
      padding_top + inner_height,
      "stroke");

  target->finishGroup();
}

void Canvas::renderBottomAxis(
    RenderTarget* target,
    AxisDefinition* axis,
    std::tuple<int, int, int, int>* padding,
    int bottom) const {
  int padding_left = std::get<3>(*padding);
  int inner_width = width_ - std::get<1>(*padding) - padding_left;

  bottom += kAxisPadding;
  target->beginGroup("axis bottom");

  /* draw title */
  if (axis->hasTitle()) {
    target->drawText(
        axis->getTitle(),
        padding_left + inner_width* 0.5f,
        height_ - bottom,
        "middle",
        "no-change",
        "title");

    bottom += kAxisTitleLength;
  }

  /* draw labels */
  if (axis->hasLabels()) {
    bottom += kAxisLabelHeight; // FIXPAUL: calculate label width?

    for (const auto& label : axis->getLabels()) {
      auto tick_x = padding_left + inner_width * label.first;

      target->drawText(
          label.second,
          tick_x,
          height_ - bottom + (kTickLength * 2),
          "middle",
          "text-before-edge",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_x = padding_left + inner_width * tick;

    target->drawLine(
        tick_x,
        height_ - bottom,
        tick_x,
        height_ - bottom - kTickLength,
        "tick");
  }

  /* draw stroke */
  target->drawLine(
      padding_left,
      height_ - bottom,
      padding_left + inner_width,
      height_ - bottom,
      "stroke");

  target->finishGroup();
}

void Canvas::renderLeftAxis(
    RenderTarget* target,
    AxisDefinition* axis,
    std::tuple<int, int, int, int>* padding,
    int left) const {
  int padding_top = std::get<0>(*padding);
  int inner_height = height_ - std::get<2>(*padding) - padding_top;

  left += kAxisPadding;
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
  if (axis->hasLabels()) {
    left += kAxisLabelWidth; // FIXPAUL: calculate label width?

    for (const auto& label : axis->getLabels()) {
      auto tick_y = padding_top + inner_height * (1.0 - label.first);

      target->drawText(
          label.second,
          left - (kTickLength * 2),
          tick_y,
          "end",
          "middle",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_y = padding_top + inner_height * (1.0 - tick);

    target->drawLine(
        left,
        tick_y,
        left + kTickLength,
        tick_y,
        "tick");
  }

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

AxisDefinition* Canvas::addAxis(
    AxisDefinition::kPosition axis_position,
    Domain* domain) {
  auto axis = new AxisDefinition(axis_position, domain);
  axes_.emplace_back(axis);
  return axis;
}

AxisDefinition* Canvas::addAxis(AxisDefinition::kPosition axis_position) {
  auto axis = new AxisDefinition(axis_position);
  axes_.emplace_back(axis);
  return axis;
}

}
}

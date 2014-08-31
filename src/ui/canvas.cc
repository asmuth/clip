/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <assert.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/rendertarget.h>
#include <fnordmetric/ui/domain.h>
#include <fnordmetric/ui/rendertarget.h>
#include <fnordmetric/ui/svgtarget.h>

namespace fnordmetric {
namespace ui {

Canvas::Canvas() :
    width_(750),
    height_(300) {}

void Canvas::render(RenderTarget* target) const {
  // FIXPAUL: initialize from rendertarget
  Viewport viewport(width_, height_);

  target->beginChart(width_, height_, "chart bar horizontal"); 
  renderAxes(target, &viewport);
  renderCharts(target, &viewport);
  target->finishChart();
}

void Canvas::renderCharts(RenderTarget* target, Viewport* viewport) const {
  for (const auto& drawable : drawables_) {
    drawable->render(target, viewport);
  }
}

void Canvas::renderAxes(RenderTarget* target, Viewport* viewport) const {
  std::tuple<int, int, int, int> padding;
  std::vector<std::pair<int, AxisDefinition*>> top;
  std::vector<std::pair<int, AxisDefinition*>> right;
  std::vector<std::pair<int, AxisDefinition*>> bottom;
  std::vector<std::pair<int, AxisDefinition*>> left;

  for (const auto& axis : axes_) {
    switch (axis->getPosition()) {

      case AxisDefinition::TOP: {
        top.emplace_back(std::get<0>(padding), axis.get());
        std::get<0>(padding) += kAxisPadding;
        std::get<0>(padding) += axis->hasLabels() ? kAxisLabelHeight : 0;
        std::get<0>(padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::RIGHT: {
        right.emplace_back(std::get<1>(padding), axis.get());
        std::get<1>(padding) += kAxisPadding;
        std::get<1>(padding) += axis->hasLabels() ? kAxisLabelWidth : 0;
        std::get<1>(padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::BOTTOM: {
        bottom.emplace_back(std::get<2>(padding), axis.get());
        std::get<2>(padding) += kAxisPadding;
        std::get<2>(padding) += axis->hasLabels() ? kAxisLabelHeight : 0;
        std::get<2>(padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

      case AxisDefinition::LEFT: {
        left.emplace_back(std::get<3>(padding), axis.get());
        std::get<3>(padding) += kAxisPadding;
        std::get<3>(padding) += axis->hasLabels() ? kAxisLabelWidth : 0;
        std::get<3>(padding) += axis->hasTitle() ? kAxisTitleLength : 0;
        break;
      }

    }
  }

  if (std::get<0>(padding) < kAxisLabelHeight * 0.5f) {
    std::get<0>(padding) += kAxisLabelHeight * 0.5f;
  }

  if (std::get<1>(padding) < kAxisLabelWidth * 0.5f) {
    std::get<1>(padding) += kAxisLabelWidth * 0.5f;
  }

  if (std::get<2>(padding) < kAxisLabelHeight * 0.5f) {
    std::get<2>(padding) += kAxisLabelHeight * 0.5f;
  }

  if (std::get<3>(padding) < kAxisLabelWidth * 0.5f) {
    std::get<3>(padding) += kAxisLabelWidth * 0.5f;
  }

  viewport->setPadding(padding);

  for (const auto& placement : top) {
    renderTopAxis(target, viewport, placement.second, placement.first);
  }

  for (const auto& placement : right) {
    renderRightAxis(target, viewport, placement.second, placement.first);
  }

  for (const auto& placement : bottom) {
    renderBottomAxis(target, viewport, placement.second, placement.first);
  }

  for (const auto& placement : left) {
    renderLeftAxis(target, viewport, placement.second, placement.first);
  }
}

void Canvas::renderTopAxis(
    RenderTarget* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int top) const {
  int padding_left = viewport->paddingLeft();
  int inner_width = viewport->innerWidth();

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
    Viewport* viewport,
    AxisDefinition* axis,
    int right) const {
  int padding_top = viewport->paddingTop();
  int inner_height = viewport->innerHeight();

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
    Viewport* viewport,
    AxisDefinition* axis,
    int bottom) const {
  int padding_left = viewport->paddingLeft();
  int inner_width = viewport->innerWidth();

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
    Viewport* viewport,
    AxisDefinition* axis,
    int left) const {
  int padding_top = viewport->paddingTop();
  int inner_height = viewport->innerHeight();

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
  auto output = util::OutputStream::getStdout();
  SVGTarget target(output.get());
  render(&target);
  return "fnord";
}

AxisDefinition* Canvas::addAxis(AxisDefinition::kPosition position) {
  axes_.emplace_back(new AxisDefinition(position));
  return axes_.back().get();
}

}
}

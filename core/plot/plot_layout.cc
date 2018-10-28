/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include "../graphics/rendertarget.h"
#include "plot_layout.h"
#include "domain.h"

namespace signaltk {
namespace chart {

PlotLayout::PlotLayout() :
    width_(800),
    height_(320) {}

void PlotLayout::setDimensions(int width, int height) {
  if (width > 0) {
    width_ = width;
  }

  if (height > 0) {
    height_ = height;
  }
}

void PlotLayout::render(RenderTarget* target) const {
  // FIXPAUL: initialize from rendertarget
  Viewport viewport(width_, height_);

  target->beginChart(width_, height_, "fm-chart");
  renderAxes(target, &viewport);
  renderGrids(target, &viewport);
  target->finishChart();
}

void PlotLayout::renderAxes(RenderTarget* target, Viewport* viewport) const {
  std::tuple<int, int, int, int> padding = viewport->padding();
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

void PlotLayout::renderTopAxis(
    RenderTarget* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int top) const {
  StrokeStyle style;

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
          top - kAxisLabelHeight * 0.5f,
          "middle",
          "central",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_x = padding_left + inner_width * tick;

    target->strokeLine(
        tick_x,
        top,
        tick_x,
        top + kTickLength,
        style);
  }

  /* draw stroke */
  target->strokeLine(
      padding_left,
      top,
      padding_left + inner_width,
      top,
      style);

  target->finishGroup();
}


void PlotLayout::renderRightAxis(
    RenderTarget* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int right) const {
  StrokeStyle style;

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

    target->strokeLine(
        width_ - right,
        tick_y,
        width_ - right - kTickLength,
        tick_y,
        style);
  }

  /* draw stroke */
  target->strokeLine(
      width_ - right,
      padding_top,
      width_ - right,
      padding_top + inner_height,
      style);

  target->finishGroup();
}

void PlotLayout::renderBottomAxis(
    RenderTarget* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int bottom) const {
  StrokeStyle style;

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
          height_ - bottom + kAxisLabelHeight * 0.5f,
          "middle",
          "central",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_x = padding_left + inner_width * tick;

    target->strokeLine(
        tick_x,
        height_ - bottom,
        tick_x,
        height_ - bottom - kTickLength,
        style);
  }

  /* draw stroke */
  target->strokeLine(
      padding_left,
      height_ - bottom,
      padding_left + inner_width,
      height_ - bottom,
      style);

  target->finishGroup();
}

void PlotLayout::renderLeftAxis(
    RenderTarget* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int left) const {
  StrokeStyle style;

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

    target->strokeLine(
        left,
        tick_y,
        left + kTickLength,
        tick_y,
        style);
  }

  /* draw stroke */
  target->strokeLine(
      left,
      padding_top,
      left,
      padding_top + inner_height,
      style);

  target->finishGroup();
}

void PlotLayout::renderGrids(RenderTarget* target, Viewport* viewport) const {
  StrokeStyle style;

  for (const auto& grid : grids_) {
    switch (grid->placement()) {

      case GridDefinition::GRID_HORIZONTAL:
        target->beginGroup("grid horizontal");
        for (const auto& tick : grid->ticks()) {
          auto line_y = viewport->paddingTop() +
              viewport->innerHeight() * (1.0 - tick);

          target->strokeLine(
              viewport->paddingLeft(),
              line_y,
              viewport->paddingLeft() + viewport->innerWidth(),
              line_y,
              style);
        }
        target->finishGroup();
        break;

      case GridDefinition::GRID_VERTICAL:
        target->beginGroup("grid vertical");
        for (const auto& tick : grid->ticks()) {
          auto line_x = viewport->paddingLeft() + viewport->innerWidth() * tick;

          target->strokeLine(
              line_x,
              viewport->paddingTop(),
              line_x,
              viewport->paddingTop() + viewport->innerHeight(),
              style);
        }
        target->finishGroup();
        break;

    }
  }
}

AxisDefinition* PlotLayout::addAxis(AxisDefinition::kPosition position) {
  axes_.emplace_back(new AxisDefinition(position));
  return axes_.back().get();
}

GridDefinition* PlotLayout::addGrid(GridDefinition::kPlacement placement) {
  grids_.emplace_back(new GridDefinition(placement));
  return grids_.back().get();
}

}
}

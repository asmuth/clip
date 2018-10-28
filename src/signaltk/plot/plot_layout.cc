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
#include "../core/layer.h"
#include "../core/brush.h"
#include "../core/text.h"
#include "plot_layout.h"
#include "domain.h"

namespace signaltk {
namespace chart {

// FIXME
int estimateTextLength(const std::string& str) {
  return str.size() * kCharWidth;
}

/*
void renderAxes(Layer* target, Viewport* viewport) {
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

*/

void renderTopAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int top) {
  StrokeStyle style;

  int padding_left = viewport->paddingLeft();
  int inner_width = viewport->innerWidth();

  top += kAxisPadding;

  /* draw title */
  if (axis->hasTitle()) {
    drawText(target,
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

      drawText(target,
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

    strokeLine(target,
        tick_x,
        top,
        tick_x,
        top + kTickLength,
        style);
  }

  /* draw stroke */
  strokeLine(target,
      padding_left,
      top,
      padding_left + inner_width,
      top,
      style);

}


void renderRightAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int right) {
  StrokeStyle style;

  int padding_top = viewport->paddingTop();
  int inner_height = viewport->innerHeight();

  right += kAxisPadding;

  /* draw title */
  if (axis->hasTitle()) {
    right += kAxisTitleLength;
    drawText(target,
        axis->getTitle(),
        viewport->width_ - right,
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

      drawText(target,
          label.second,
          viewport->width_ - right + (kTickLength * 2),
          tick_y,
          "start",
          "middle",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_y = padding_top + inner_height * (1.0 - tick);

    strokeLine(target,
        viewport->width_ - right,
        tick_y,
        viewport->width_ - right - kTickLength,
        tick_y,
        style);
  }

  /* draw stroke */
  strokeLine(target,
      viewport->width_ - right,
      padding_top,
      viewport->width_ - right,
      padding_top + inner_height,
      style);

}

void renderBottomAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int bottom) {
  StrokeStyle style;

  int padding_left = viewport->paddingLeft();
  int inner_width = viewport->innerWidth();

  bottom += kAxisPadding;

  /* draw title */
  if (axis->hasTitle()) {
    drawText(target,
        axis->getTitle(),
        padding_left + inner_width* 0.5f,
        viewport->height_ - bottom,
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

      drawText(target,
          label.second,
          tick_x,
          viewport->height_ - bottom + kAxisLabelHeight * 0.5f,
          "middle",
          "central",
          "label");
    }
  }

  /* draw ticks */
  for (const auto& tick : axis->getTicks()) {
    auto tick_x = padding_left + inner_width * tick;

    strokeLine(target,
        tick_x,
        viewport->height_ - bottom,
        tick_x,
        viewport->height_ - bottom - kTickLength,
        style);
  }

  /* draw stroke */
  strokeLine(target,
      padding_left,
      viewport->height_ - bottom,
      padding_left + inner_width,
      viewport->height_ - bottom,
      style);

}

void renderLeftAxis(
    Layer* target,
    Viewport* viewport,
    AxisDefinition* axis,
    int left) {
  StrokeStyle style;

  int padding_top = viewport->paddingTop();
  int inner_height = viewport->innerHeight();

  left += kAxisPadding;

  /* draw title */
  if (axis->hasTitle()) {
    drawText(target,
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

      drawText(target,
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

    strokeLine(target,
        left,
        tick_y,
        left + kTickLength,
        tick_y,
        style);
  }

  /* draw stroke */
  strokeLine(target,
      left,
      padding_top,
      left,
      padding_top + inner_height,
      style);

}


void renderGrid(
    const GridDefinition& grid,
    const Viewport& viewport,
    Layer* target) {
  StrokeStyle style;

  switch (grid.placement()) {

    case GridDefinition::GRID_HORIZONTAL:
      for (const auto& tick : grid.ticks()) {
        auto line_y = viewport.paddingTop() +
            viewport.innerHeight() * (1.0 - tick);

        strokeLine(
            target,
            viewport.paddingLeft(),
            line_y,
            viewport.paddingLeft() + viewport.innerWidth(),
            line_y,
            style);
      }
      break;

    case GridDefinition::GRID_VERTICAL:
      for (const auto& tick : grid.ticks()) {
        auto line_x = viewport.paddingLeft() + viewport.innerWidth() * tick;

        strokeLine(
            target,
            line_x,
            viewport.paddingTop(),
            line_x,
            viewport.paddingTop() + viewport.innerHeight(),
            style);
      }
      break;

  }
}

}
}

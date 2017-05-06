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
#include "cplot/canvas.h"
#include "cplot/rendertarget.h"
#include "cplot/domain.h"
#include "cplot/svgtarget.h"

namespace stx {
namespace chart {

Canvas::Canvas() :
    width_(800),
    height_(320) {}

void Canvas::setTitle(const std::string& title) {
  title_ = title;
}

void Canvas::setSubtitle(const std::string& subtitle) {
  subtitle_ = subtitle;
}

void Canvas::setDimensions(int width, int height) {
  if (width > 0) {
    width_ = width;
  }

  if (height > 0) {
    height_ = height;
  }
}

void Canvas::render(RenderTarget* target) const {
  // FIXPAUL: initialize from rendertarget
  Viewport viewport(width_, height_);

  target->beginChart(width_, height_, "fm-chart");
  renderTitle(target, &viewport);
  renderOutsideLegends(target, &viewport);
  renderAxes(target, &viewport);
  renderGrids(target, &viewport);
  renderInsideLegends(target, &viewport);
  renderCharts(target, &viewport);
  target->finishChart();
}

void Canvas::renderTitle(RenderTarget* target, Viewport* viewport) const {
  if (title_.size() > 0) {
    target->drawText(
        title_,
        viewport->paddingLeft() + viewport->innerWidth() * 0.5f,
        viewport->paddingTop(),
        "middle",
        "text-before-edge",
        "chart-title");

    viewport->setPaddingTop(viewport->paddingTop() + kTitleLineHeight);
  }

  if (subtitle_.size() > 0) {
    target->drawText(
        subtitle_,
        viewport->paddingLeft() + viewport->innerWidth() * 0.5f,
        viewport->paddingTop(),
        "middle",
        "text-before-edge",
        "chart-subtitle");

    viewport->setPaddingTop(viewport->paddingTop() + kSubtitleLineHeight);
  }
}

void Canvas::renderCharts(RenderTarget* target, Viewport* viewport) const {
  for (const auto& drawable : drawables_) {
    drawable->render(target, viewport);
  }
}

void Canvas::renderAxes(RenderTarget* target, Viewport* viewport) const {
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
          top - kAxisLabelHeight * 0.5f,
          "middle",
          "central",
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
          height_ - bottom + kAxisLabelHeight * 0.5f,
          "middle",
          "central",
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

void Canvas::renderOutsideLegends(
    RenderTarget* target,
    Viewport* viewport) const {
  for (const auto& legend : legends_) {
    if (legend->placement() != LegendDefinition::LEGEND_OUTSIDE) {
      continue;
    }

    target->beginGroup("legend");

    switch (legend->verticalPosition()) {
      case LegendDefinition::LEGEND_TOP: {
        switch (legend->horizontalPosition()) {
          case LegendDefinition::LEGEND_LEFT:
            renderLeftLegend(
                target,
                viewport,
                legend.get(),
                kLegendOutsideHorizPadding,
                false,
                true);
            break;
          case LegendDefinition::LEGEND_RIGHT:
            renderRightLegend(
                target,
                viewport,
                legend.get(),
                kLegendOutsideHorizPadding,
                false,
                true);
            break;
          }

        viewport->setPaddingTop(
            viewport->paddingTop() + kLegendOutsideVertPadding);
        break;
      }

      case LegendDefinition::LEGEND_BOTTOM: {
        switch (legend->horizontalPosition()) {
          case LegendDefinition::LEGEND_LEFT:
            renderLeftLegend(
                target,
                viewport,
                legend.get(),
                kLegendOutsideHorizPadding,
                true,
                true);
            break;
          case LegendDefinition::LEGEND_RIGHT:
            renderRightLegend(
                target,
                viewport,
                legend.get(),
                kLegendOutsideHorizPadding,
                true,
                true);
            break;

          }

        viewport->setPaddingBottom(
            viewport->paddingBottom() + kLegendOutsideVertPadding);
        break;
      }
    }

    target->finishGroup();
  }
}

void Canvas::renderInsideLegends(
    RenderTarget* target,
    Viewport* viewport) const {
  auto orig_padding = viewport->padding();

  for (const auto& legend : legends_) {
    if (legend->placement() != LegendDefinition::LEGEND_INSIDE) {
      continue;
    }

    target->beginGroup("legend");

    viewport->setPaddingTop(viewport->paddingTop() + kLegendInsideVertPadding);
    viewport->setPaddingBottom(
        viewport->paddingBottom() + kLegendInsideVertPadding);

    switch (legend->horizontalPosition()) {
      case LegendDefinition::LEGEND_LEFT:
        renderLeftLegend(
            target,
            viewport,
            legend.get(),
            kLegendOutsideHorizPadding,
            legend->verticalPosition() == LegendDefinition::LEGEND_BOTTOM,
            false);
        break;
      case LegendDefinition::LEGEND_RIGHT:
        renderRightLegend(
            target,
            viewport,
            legend.get(),
            kLegendOutsideHorizPadding,
            legend->verticalPosition() == LegendDefinition::LEGEND_BOTTOM,
            false);
        break;
      }

    target->finishGroup();
  }

  viewport->setPadding(orig_padding);
}

void Canvas::renderRightLegend(
    RenderTarget* target,
    Viewport* viewport,
    LegendDefinition* legend,
    double horiz_padding,
    bool bottom,
    bool outside) const {
  std::string title = legend->title();

  double height;
  if (bottom) {
    height = viewport->paddingTop() + viewport->innerHeight()  -
      kLegendLineHeight * 0.5f;
  } else {
    height = viewport->paddingTop();
  }

  target->drawText(
    title,
    viewport->paddingLeft() + horiz_padding,
    height,
    "start",
    bottom ? "text-after-edge" : "text-before-edge",
    "title");

  auto lx = viewport->paddingLeft() + viewport->innerWidth() - horiz_padding;
  auto lx_boundary = viewport->paddingLeft() + horiz_padding +
    estimateTextLength(title) + kLegendLabelPadding;

  for (const auto& entry : legend->entries()) {
    auto this_len = estimateTextLength(std::get<0>(entry)) +
        kLegendLabelPadding;

    /* line wrap */
    if (lx - this_len < lx_boundary) {
      lx = viewport->paddingLeft() + viewport->innerWidth() - horiz_padding;
      height += bottom ? -1 * kLegendLineHeight : kLegendLineHeight;
      lx_boundary = viewport->paddingLeft() + horiz_padding;
    }

    auto ly = bottom ?
        height - kLegendPointSize * 0.4f :
        height + kLegendPointSize * 2.0f;

    target->drawPoint(
        lx,
        ly,
        std::get<2>(entry),
        kLegendPointSize,
        std::get<1>(entry),
        "point");

    target->drawText(
      std::get<0>(entry),
      lx - kLegendPointWidth,
      ly,
      "end",
      "central",
      "label");

    lx -= this_len;
  }

  if (bottom) {
    if (outside) {
      height -= kLegendLineHeight;
    }

    viewport->setPaddingBottom(
        viewport->innerHeight() + viewport->paddingTop() +
        viewport->paddingBottom() - height);
  } else {
    height += kLegendLineHeight;
    viewport->setPaddingTop(height);
  }
}

void Canvas::renderLeftLegend(
    RenderTarget* target,
    Viewport* viewport,
    LegendDefinition* legend,
    double horiz_padding,
    bool bottom,
    bool outside) const {
  std::string title = legend->title();

  double height;
  if (bottom) {
    height = viewport->paddingTop() + viewport->innerHeight()  -
      kLegendLineHeight * 0.5f;
  } else {
    height = viewport->paddingTop();
  }

  target->drawText(
    title,
    viewport->paddingLeft() + viewport->innerWidth() - horiz_padding,
    height,
    "end",
    bottom ? "text-after-edge" : "text-before-edge",
    "title");

  auto lx = viewport->paddingLeft() + horiz_padding;
  auto lx_boundary = viewport->paddingLeft() + viewport->innerWidth() -
      horiz_padding - estimateTextLength(title) - kLegendLabelPadding;

  for (const auto& entry : legend->entries()) {
    auto this_len = estimateTextLength(std::get<0>(entry)) + 
        kLegendLabelPadding;

    /* line wrap */
    if (lx + this_len > lx_boundary) {
      lx = viewport->paddingLeft() + horiz_padding;
      lx_boundary = viewport->paddingLeft() + viewport->innerWidth() -
          horiz_padding;
      height += bottom ? -1 * kLegendLineHeight : kLegendLineHeight;
    }

    auto ly = bottom ?
        height - kLegendPointSize * 0.4f :
        height + kLegendPointSize * 2.0f;

    target->drawPoint(
        lx,
        ly,
        std::get<2>(entry),
        kLegendPointSize,
        std::get<1>(entry),
        "point");

    target->drawText(
      std::get<0>(entry),
      lx + kLegendPointWidth,
      ly,
      "start",
      "central",
      "label");

    lx += this_len;
  }

  if (bottom) {
    if (outside) {
      height -= kLegendLineHeight;
    }

    viewport->setPaddingBottom(
        viewport->innerHeight() + viewport->paddingTop() +
        viewport->paddingBottom() - height);
  } else {
    height += kLegendLineHeight;
    viewport->setPaddingTop(height);
  }
}

void Canvas::renderGrids(RenderTarget* target, Viewport* viewport) const {
  for (const auto& grid : grids_) {
    switch (grid->placement()) {

      case GridDefinition::GRID_HORIZONTAL:
        target->beginGroup("grid horizontal");
        for (const auto& tick : grid->ticks()) {
          auto line_y = viewport->paddingTop() +
              viewport->innerHeight() * (1.0 - tick);

          target->drawLine(
              viewport->paddingLeft(),
              line_y,
              viewport->paddingLeft() + viewport->innerWidth(),
              line_y,
              "gridline");
        }
        target->finishGroup();
        break;

      case GridDefinition::GRID_VERTICAL:
        target->beginGroup("grid vertical");
        for (const auto& tick : grid->ticks()) {
          auto line_x = viewport->paddingLeft() + viewport->innerWidth() * tick;

          target->drawLine(
              line_x,
              viewport->paddingTop(),
              line_x,
              viewport->paddingTop() + viewport->innerHeight(),
              "gridline");
        }
        target->finishGroup();
        break;

    }
  }
}

AxisDefinition* Canvas::addAxis(AxisDefinition::kPosition position) {
  axes_.emplace_back(new AxisDefinition(position));
  return axes_.back().get();
}

GridDefinition* Canvas::addGrid(GridDefinition::kPlacement placement) {
  grids_.emplace_back(new GridDefinition(placement));
  return grids_.back().get();
}

LegendDefinition* Canvas::addLegend(
    LegendDefinition::kVerticalPosition vert_pos,
    LegendDefinition::kHorizontalPosition horiz_pos,
    LegendDefinition::kPlacement placement,
    const std::string& title) {
  legends_.emplace_back(
      new LegendDefinition(vert_pos, horiz_pos, placement, title));
  return legends_.back().get();
}

LegendDefinition* Canvas::legend() const {
  if (legends_.size() == 0) {
    return nullptr;
  } else {
    return legends_.back().get();
  }
}

}
}

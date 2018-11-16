/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include <stdlib.h>
#include "../core/layer.h"
#include "../graphics/svgtarget.h"
#include "canvas.h"
#include "domain.h"

namespace plotfx {
namespace chart {

Legend::Legend() :
    width_(800),
    height_(320) {}

void Legend::renderOutsideLegends(
    Layer* target,
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

void Legend::renderInsideLegends(
    Layer* target,
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

void Legend::renderRightLegend(
    Layer* target,
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

void Legend::renderLeftLegend(
    Layer* target,
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

LegendDefinition* Legend::addLegend(
    LegendDefinition::kVerticalPosition vert_pos,
    LegendDefinition::kHorizontalPosition horiz_pos,
    LegendDefinition::kPlacement placement,
    const std::string& title) {
  legends_.emplace_back(
      new LegendDefinition(vert_pos, horiz_pos, placement, title));
  return legends_.back().get();
}

LegendDefinition* Legend::legend() const {
  if (legends_.size() == 0) {
    return nullptr;
  } else {
    return legends_.back().get();
  }
}

}
}

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
#include "graphics/layer.h"
#include "common/domain.h"
#include "common/config_helpers.h"
#include "legend.h"

namespace plotfx {

static const double kDefaultLabelFontSizeEM = 1;
static const double kDefaultPaddingHorizEM = 2.4;
static const double kDefaultPaddingVertEM = 1.2;
static const double kDefaultItemPaddingHorizEM = 2.4;
static const double kDefaultItemPaddingVertEM = 1.0;

LegendConfig::LegendConfig() :
    vert_pos(LEGEND_TOP),
    horiz_pos(LEGEND_LEFT),
    placement(LEGEND_INSIDE) {}

void LegendConfig::addEntry(
    const std::string& name,
    const Color& color,
    const std::string& shape /* = "circle" */) {
  entries.emplace_back(name, color, shape);
}

/*
void Legend::renderOutsideLegends(
    Layer* target,
    const Rectangle& clip) const {
  for (const auto& legend : legends_) {
    if (legend->placement() != LegendConfig::LEGEND_OUTSIDE) {
      continue;
    }

    target->beginGroup("legend");

    switch (legend->verticalPosition()) {
      case LegendConfig::LEGEND_TOP: {
        switch (legend->horizontalPosition()) {
          case LegendConfig::LEGEND_LEFT:
            renderLeftLegend(
                target,
                viewport,
                legend.get(),
                kLegendOutsideHorizPadding,
                false,
                true);
            break;
          case LegendConfig::LEGEND_RIGHT:
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

      case LegendConfig::LEGEND_BOTTOM: {
        switch (legend->horizontalPosition()) {
          case LegendConfig::LEGEND_LEFT:
            renderLeftLegend(
                target,
                viewport,
                legend.get(),
                kLegendOutsideHorizPadding,
                true,
                true);
            break;
          case LegendConfig::LEGEND_RIGHT:
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
    const Rectangle& clip) const {
  auto orig_padding = viewport->padding();

  for (const auto& legend : legends_) {
    if (legend->placement() != LegendConfig::LEGEND_INSIDE) {
      continue;
    }

    target->beginGroup("legend");

    viewport->setPaddingTop(viewport->paddingTop() + kLegendInsideVertPadding);
    viewport->setPaddingBottom(
        viewport->paddingBottom() + kLegendInsideVertPadding);

    switch (legend->horizontalPosition()) {
      case LegendConfig::LEGEND_LEFT:
        renderLeftLegend(
            target,
            viewport,
            legend.get(),
            kLegendOutsideHorizPadding,
            legend->verticalPosition() == LegendConfig::LEGEND_BOTTOM,
            false);
        break;
      case LegendConfig::LEGEND_RIGHT:
        renderRightLegend(
            target,
            viewport,
            legend.get(),
            kLegendOutsideHorizPadding,
            legend->verticalPosition() == LegendConfig::LEGEND_BOTTOM,
            false);
        break;
      }

    target->finishGroup();
  }

  viewport->setPadding(orig_padding);
}

void Legend::renderRightLegend(
    Layer* target,
    const Rectangle& clip,
    LegendConfig* legend,
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
    const Rectangle& clip,
    LegendConfig* legend,
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

LegendConfig* Legend::addLegend(
    LegendConfig::kVerticalPosition vert_pos,
    LegendConfig::kHorizontalPosition horiz_pos,
    LegendConfig::kPlacement placement,
    const std::string& title) {
  legends_.emplace_back(
      new LegendConfig(vert_pos, horiz_pos, placement, title));
  return legends_.back().get();
}

LegendConfig* Legend::legend() const {
  if (legends_.size() == 0) {
    return nullptr;
  } else {
    return legends_.back().get();
  }
}
*/
ReturnCode legend_draw(
    const LegendConfig& legend,
    const Rectangle& bbox,
    Layer* layer) {
  auto font_size = from_em(kDefaultLabelFontSizeEM, layer->font_size);

  double padding_horiz = measure_or(
      legend.padding_horiz,
      from_em(kDefaultPaddingHorizEM, font_size));

  double padding_vert = measure_or(
      legend.padding_vert,
      from_em(kDefaultPaddingVertEM, font_size));

  double padding_item_horiz = measure_or(
      legend.padding_horiz,
      from_em(kDefaultItemPaddingHorizEM, font_size));

  double padding_item_vert = measure_or(
      legend.padding_vert,
      from_em(kDefaultItemPaddingVertEM, font_size));

  double point_size = 5;
  double line_height = 14;

  double sx = bbox.x + padding_horiz;
  double sy = bbox.y + padding_vert + line_height / 2;

  for (const auto& e : legend.entries) {
    const auto& label_text = std::get<0>(e);

    {
      FillStyle style;
      style.color = std::get<1>(e);
      Path path;
      path.moveTo(sx + point_size, sy);
      path.arcTo(sx, sy, point_size, 0, M_PI * 2);
      fillPath(layer, path, style);
      sx += point_size * 2.4;
    }

    {
      TextStyle style;
      style.color = legend.text_color;
      style.font = legend.font;
      style.font_size = font_size;

      Rectangle label_bbox;
      auto rc = text::text_measure_span(
          label_text,
          style.font,
          style.font_size,
          layer->dpi,
          layer->text_shaper.get(),
          &label_bbox);

      if (auto rc = drawTextLabel(
            label_text,
            Point(sx, sy),
            HAlign::LEFT,
            VAlign::CENTER,
            style,
            layer); rc != OK) {
        return rc;
      }

      sx += label_bbox.w + padding_item_horiz;
    }
  }

  return OK;
}

ReturnCode legend_configure(
    const Document& doc,
    const plist::PropertyList& plist,
    LegendConfig* config) {
  config->font = doc.font_sans;
  config->border_color = doc.border_color;
  config->text_color = doc.text_color;

  static const ParserDefinitions pdefs = {
    {"legend-text-color", std::bind(&configure_color, std::placeholders::_1, &config->text_color)},
    {"legend-border-color", std::bind(&configure_color, std::placeholders::_1, &config->border_color)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

} // namespace plotfx


/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "source/core/scale.h"
#include "source/config_helpers.h"
#include "source/utils/algo.h"
#include "legend.h"

using namespace std::placeholders;

namespace plotfx {

static const double kDefaultLabelFontSizeEM = 1;
static const double kDefaultPaddingHorizEM = 2.4;
static const double kDefaultPaddingVertEM = 1.6;
static const double kDefaultItemPaddingHorizEM = 2.4;
static const double kDefaultItemPaddingVertEM = 1.0;

LegendConfig::LegendConfig() :
    key(LEGEND_DEFAULT),
    placement(LegendPlacement::INSIDE),
    position_horiz(HAlign::LEFT),
    position_vert(VAlign::TOP) {}

ReturnCode legend_layout_items(
    const LegendConfig& legend,
    const std::vector<std::string>& legend_items,
    Point origin,
    std::function<ReturnCode (size_t idx, Point p)> on_label,
    const Layer* layer,
    Rectangle* bbox) {
  auto font_size = from_em(kDefaultLabelFontSizeEM, layer->font_size);
  auto padding_item_right = measure_or(
      legend.item_margins[1],
      from_em(kDefaultItemPaddingHorizEM, font_size));

  double point_size = 5; // TODO
  double line_height = font_size; // TODO

  double sx = origin.x;
  double sy = origin.y;

  for (size_t idx = 0; idx < legend_items.size(); ++idx) {
    const auto& label_text = legend_items[idx];

    if (on_label) {
      if (auto rc = on_label(idx, Point(sx, sy + line_height / 2)); !rc) {
        return rc;
      }
    }

    TextStyle style;
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

    if (rc != OK) {
      return rc;
    }

    sx += point_size * 2.8; // TODO
    sx += label_bbox.w;

    if (idx + 1 < legend_items.size()) {
      sx += padding_item_right;
    }
  }

  sy += line_height;

  if (bbox) {
    *bbox = Rectangle(origin.x, origin.y, sx - origin.x, sy - origin.y);
  }

  return OK;
}

ReturnCode legend_draw_inside(
    const LegendConfig& legend,
    const Rectangle& bbox,
    Layer* layer) {
  auto font_size = from_em(kDefaultLabelFontSizeEM, layer->font_size);

  double padding_left = measure_or(
      legend.margins[3],
      from_em(kDefaultPaddingHorizEM, font_size));

  double padding_right = measure_or(
      legend.margins[1],
      from_em(kDefaultPaddingHorizEM, font_size));

  double padding_top = measure_or(
      legend.margins[0],
      from_em(kDefaultPaddingVertEM, font_size));

  double padding_bottom = measure_or(
      legend.margins[2],
      from_em(kDefaultPaddingVertEM, font_size));

  double padding_item_right = measure_or(
      legend.item_margins[1],
      from_em(kDefaultItemPaddingHorizEM, font_size));

  double point_size = 5; // TODO
  auto draw_label = [&] (size_t idx, Point pos) {
    const auto& label_text = legend.items[idx];

    const auto& color = legend.colors.empty()
        ? Color{}
        : legend.colors[idx % legend.colors.size()];

    {
      FillStyle style;
      style.color = color;
      Path path;
      path.moveTo(pos.x + point_size + point_size / 2, pos.y);
      path.arcTo(pos.x + point_size / 2, pos.y, point_size, 0, M_PI * 2);
      fillPath(layer, path, style);
      pos.x += point_size * 2.8; // TODO
    }

    {
      TextStyle style;
      style.color = legend.text_color;
      style.font = legend.font;
      style.font_size = font_size;

      if (auto rc = drawTextLabel(
            label_text,
            pos,
            HAlign::LEFT,
            VAlign::CENTER,
            style,
            layer); rc != OK) {
        return rc;
      }
    }

    return OK;
  };

  Rectangle content_bbox;
  if (auto rc = legend_layout_items(
      legend,
      legend.items,
      Point(0, 0),
      nullptr,
      layer,
      &content_bbox);
      !rc) {
    return rc;
  }

  Point origin(0, 0);

  switch (legend.position_horiz) {
    case HAlign::LEFT:
      origin.x = std::min(
          bbox.x + padding_left,
          bbox.x + bbox.w);
      break;
    case HAlign::RIGHT:
      origin.x = std::max(
          bbox.x + bbox.w - content_bbox.w - padding_right,
          bbox.x);
      break;
    case HAlign::CENTER:
      origin.x = bbox.x + bbox.w / 2 - content_bbox.w / 2;
      break;
  }

  switch (legend.position_vert) {
    case VAlign::TOP:
      origin.y = bbox.y + padding_top;
      break;
    case VAlign::BOTTOM:
      origin.y = bbox.y + bbox.h - content_bbox.h - padding_bottom;
      break;
    case VAlign::CENTER:
      origin.y = bbox.y + bbox.h / 2;
      break;
  }

  if (auto rc = legend_layout_items(
      legend,
      legend.items,
      origin,
      draw_label,
      layer,
      nullptr);
      !rc) {
    return rc;
  }

  return OK;
}

ReturnCode legend_configure_position(
    const plist::Property& prop,
    LegendPlacement* placement,
    HAlign* position_horiz,
    VAlign* position_vert) {
  auto args = plist::flatten(prop);
  bool position_horiz_set = false;
  bool position_vert_set = false;
  for (const auto& prop : args) {
    if (prop == "off") {
      *placement = LegendPlacement::OFF;
      return OK;
    }

    if (prop == "inside") {
      *placement = LegendPlacement::INSIDE;
      continue;
    }

    if (prop == "outside") {
      *placement = LegendPlacement::OUTSIDE;
      continue;
    }

    if (prop == "top") {
      *position_vert = VAlign::TOP;
      position_vert_set = true;
      continue;
    }

    if (prop == "bottom") {
      *position_vert = VAlign::BOTTOM;
      position_vert_set = true;
      continue;
    }

    if (prop == "left") {
      *position_horiz = HAlign::LEFT;
      position_horiz_set = true;
      continue;
    }

    if (prop == "right") {
      *position_horiz = HAlign::RIGHT;
      position_horiz_set = true;
      continue;
    }

    if (prop == "center") {
      if (!position_horiz_set) *position_horiz = HAlign::CENTER;
      if (!position_vert_set) *position_vert = VAlign::CENTER;
      continue;
    }

    return ERROR;
  }

  return OK;
}

namespace legend {

ReturnCode draw(
    const LegendConfig& config,
    const LayoutState& layout,
    Layer* layer) {
  switch (config.placement) {
    case LegendPlacement::INSIDE:
      return legend_draw_inside(config, layout.content_box, layer);
    case LegendPlacement::OUTSIDE:
    case LegendPlacement::OFF:
    default:
      return ERROR; // TODO: not implemented
  }
}

ReturnCode layout(
    const LegendConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  switch (config.placement) {
    case LegendPlacement::INSIDE:
      /* nothing to do */
      return OK;
    case LegendPlacement::OUTSIDE:
    case LegendPlacement::OFF:
    default:
      return ERROR; // TODO: not implemented
  }
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    LegendConfig* config) {
  /* inherit defaults */
  config->font = env.font;
  config->border_color = env.border_color;
  config->text_color = env.text_color;

  /* parse properties */
  ParserDefinitions pdefs = {
    {
      "position",
      bind(
          &legend_configure_position,
          _1,
          &config->placement,
          &config->position_horiz,
          &config->position_vert)
    },
    {
      "title",
      bind(
          &configure_string,
          _1,
          &config->title)
      },
    {
      "text-color",
      bind(
          &configure_color,
          _1,
          &config->text_color)
    },
    {
      "border-color",
      bind(
          &configure_color,
          _1,
          &config->border_color)
    },
    {
      "item-margin",
      configure_multiprop({
          bind(
              &configure_measure,
              _1,
              &config->item_margins[0]),
          bind(
              &configure_measure,
              _1,
              &config->item_margins[1]),
          bind(
              &configure_measure,
              _1,
              &config->item_margins[2]),
          bind(
              &configure_measure,
              _1,
              &config->item_margins[3])
      })
    },
    {
      "item-margin-top",
      bind(
          &configure_measure,
          _1,
          &config->item_margins[0])
    },
    {
      "item-margin-right",
      bind(
          &configure_measure,
          _1,
          &config->item_margins[1])
    },
    {
      "item-margin-bottom",
      bind(
          &configure_measure,
          _1,
          &config->item_margins[2])
    },
    {
      "item-margin-left",
      bind(
          &configure_measure,
          _1,
          &config->item_margins[3])
    },
    {"color", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"colors", configure_vec<Color>(bind(&configure_color, _1, _2), &config->colors)},
    {"items", bind(&configure_strings, _1, &config->items)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc.isSuccess()) {
    return rc;
  }

  return OK;
}

} // namespace legend
} // namespace plotfx


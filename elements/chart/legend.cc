/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "legend.h"

#include "data.h"
#include "layout.h"
#include "scale.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

using namespace std::placeholders;

namespace fviz::elements::chart::legend {

const double kDefaultLabelFontSizeEM = 1;
const double kDefaultPaddingHorizEM = 1.8;
const double kDefaultPaddingVertEM = 1.4;
const double kDefaultItemPaddingHorizEM = 2.4;
const double kDefaultItemPaddingVertEM = 1.0;
const std::string LEGEND_DEFAULT = "default";

enum class LegendPlacement {
  OFF,
  INSIDE,
  OUTSIDE
};

using LegendItemList = std::vector<std::string>;

struct LegendConfig {
  LegendConfig();
  std::string key;
  Color text_color;
  Color border_color;
  FontInfo font;
  Measure padding_horiz;
  Measure padding_vert;
  Measure padding_item_horiz;
  Measure padding_item_vert;
  Measure margins[4];
  Measure item_margins[4];
  LegendPlacement placement;
  HAlign position_horiz;
  VAlign position_vert;
  std::string title;
  std::vector<std::string> items;
  std::vector<Color> colors;
  LayoutSettings layout;
};

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

ReturnCode legend_draw(
    std::shared_ptr<LegendConfig> config,
    const LayoutInfo& layout,
    Layer* layer) {
  switch (config->placement) {
    case LegendPlacement::INSIDE:
      return legend_draw_inside(*config, layout.content_box, layer);
    case LegendPlacement::OUTSIDE:
    case LegendPlacement::OFF:
    default:
      return ERROR; // TODO: not implemented
  }
}

ReturnCode legend_configure_position(
    const Expr* expr,
    LegendPlacement* placement,
    HAlign* position_horiz,
    VAlign* position_vert) {
  if (!expr || !expr_is_list(expr)) {
    return ReturnCode::errorf(
        "EARG",
        "invalid argument; expected a list but got: {}",
        "..."); // FIXME
  }

  bool position_horiz_set = false;
  bool position_vert_set = false;

  for (expr = expr_get_list(expr); expr; expr = expr_next(expr)) {
    if (expr_is_value(expr, "off")) {
      *placement = LegendPlacement::OFF;
      return OK;
    }

    if (expr_is_value(expr, "inside")) {
      *placement = LegendPlacement::INSIDE;
      continue;
    }

    if (expr_is_value(expr, "outside")) {
      *placement = LegendPlacement::OUTSIDE;
      continue;
    }

    if (expr_is_value(expr, "top")) {
      *position_vert = VAlign::TOP;
      position_vert_set = true;
      continue;
    }

    if (expr_is_value(expr, "bottom")) {
      *position_vert = VAlign::BOTTOM;
      position_vert_set = true;
      continue;
    }

    if (expr_is_value(expr, "left")) {
      *position_horiz = HAlign::LEFT;
      position_horiz_set = true;
      continue;
    }

    if (expr_is_value(expr, "right")) {
      *position_horiz = HAlign::RIGHT;
      position_horiz_set = true;
      continue;
    }

    if (expr_is_value(expr, "center")) {
      if (!position_horiz_set) *position_horiz = HAlign::CENTER;
      if (!position_vert_set) *position_vert = VAlign::CENTER;
      continue;
    }

    return ERROR;
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* inherit defaults */
  auto config = std::make_shared<LegendConfig>();
  config->font = env.font;
  config->border_color = env.border_color;
  config->text_color = env.text_color;

  /* parse exprerties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {
      "position",
      bind(
          &legend_configure_position,
          _1,
          &config->placement,
          &config->position_horiz,
          &config->position_vert)
    },
    {"title", bind( &expr_to_string, _1, &config->title)},
    {"text-color", bind(&expr_to_color, _1, &config->text_color)},
    {"border-color", bind(&expr_to_color, _1, &config->border_color)},
    {
      "item-margin",
      expr_calln_fn({
          bind(&expr_to_measure, _1, &config->item_margins[0]),
          bind(&expr_to_measure, _1, &config->item_margins[1]),
          bind(&expr_to_measure, _1, &config->item_margins[2]),
          bind(&expr_to_measure, _1, &config->item_margins[3])
      })
    },
    {"item-margin-top", bind(&expr_to_measure, _1, &config->item_margins[0])},
    {"item-margin-right", bind(&expr_to_measure, _1, &config->item_margins[1])},
    {"item-margin-bottom", bind(&expr_to_measure, _1, &config->item_margins[2])},
    {"item-margin-left", bind(&expr_to_measure, _1, &config->item_margins[3])},
    {"color", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &config->colors)},
    {"colors", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &config->colors)},
    {"items", bind(&data_load_strings, _1, &config->items)},
  });

  if (!config_rc) {
    return config_rc;
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&legend_draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::legend


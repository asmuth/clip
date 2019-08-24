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
#include "environment.h"
#include "layout.h"
#include "color_reader.h"
#include "scale.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

using namespace std::placeholders;
using std::bind;

namespace fviz::elements::legend {

struct LegendConfig {
  LegendConfig();
  HAlign position_horiz;
  VAlign position_vert;
  Measure item_row_padding;
  Measure item_column_padding;
  bool item_flow;
  std::array<Measure, 4> margins;
  std::array<Measure, 4> padding;
  std::array<StrokeStyle, 4> borders;
  std::optional<Color> background;
  std::vector<ElementRef> items;
};

struct LegendItemConfig {
  std::string elem_name;
  ExprStorage opts;
};

LegendConfig::LegendConfig() :
    position_horiz(HAlign::LEFT),
    position_vert(VAlign::TOP),
    item_flow(false) {}


void legend_normalize(
    std::shared_ptr<LegendConfig> config,
    const Page& layer) {
  for (auto& m : config->margins) {
    convert_unit_typographic(layer.dpi, layer.font_size, &m);
  }

  for (auto& m : config->padding) {
    convert_unit_typographic(layer.dpi, layer.font_size, &m);
  }

  convert_unit_typographic(layer.dpi, layer.font_size, &config->item_row_padding);
  convert_unit_typographic(layer.dpi, layer.font_size, &config->item_column_padding);
}

ReturnCode legend_layout_item_rows(
    const LegendConfig& config,
    const Page& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height,
    std::vector<Rectangle>* item_boxes) {
  /* calculate vertical size */
  double m_width = 0;
  double m_height = 0;

  for (const auto& e : config.items) {
    if (!e->size_hint) {
      continue; // FIXME warn
    }

    auto p_width = max_width;
    auto p_height = max_height;
    if (p_height) {
      p_height = *p_height - m_height;
    }

    double e_width = 0;
    double e_height = 0;
    if (auto rc = e->size_hint(layer, p_width, p_height, &e_width, &e_height); !rc) {
      return rc;
    }

    if (m_height > 0) {
      m_height += config.item_row_padding;
    }

    Rectangle item_box;
    item_box.x = 0;
    item_box.y = m_height;
    item_box.w = e_width;
    item_box.h = e_height;

    if (item_boxes) {
      item_boxes->push_back(item_box);
    }

    m_width = std::max(m_width, e_width);
    m_height += e_height;
  }

  *min_width = m_width;
  *min_height = m_height;
  return OK;
}

ReturnCode legend_layout_item_flow(
    const LegendConfig& config,
    const Page& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height,
    std::vector<Rectangle>* item_boxes) {
  /* calculate vertical size */
  double m_width = 0;
  double m_height = 0;

  size_t r_begin = item_boxes ? item_boxes->size() : 0;
  double r_width = 0;
  double r_height = 0;
  for (const auto& e : config.items) {
    if (!e->size_hint) {
      continue; // FIXME warn
    }

    double e_width = 0;
    double e_height = 0;
    if (auto rc = e->size_hint(layer, {}, {}, &e_width, &e_height); !rc) {
      return rc;
    }

    if (r_width > 0) {
      r_width += config.item_column_padding;
    }

    if (max_width && r_width + e_width > max_width) {
      m_height += r_height + config.item_row_padding;
      r_width = 0;
      r_height = 0;
      r_begin = item_boxes ? item_boxes->size() : 0;
    }

    Rectangle item_box;
    item_box.x = r_width;
    item_box.y = m_height;
    item_box.w = e_width;
    item_box.h = e_height;

    r_width += e_width;
    r_height = std::max(r_height, e_height);
    m_width = std::max(m_width, r_width);

    if (item_boxes) {
      item_boxes->push_back(item_box);

      // not a problem since rows are O(10^1)
      for (size_t i = r_begin; i < item_boxes->size(); ++i) {
        item_boxes->at(i).h = r_height;
      }
    }
  }

  *min_width = m_width;
  *min_height = m_height + r_height;
  return OK;
}

ReturnCode legend_layout(
    std::shared_ptr<LegendConfig> config,
    const Page& layer,
    std::optional<double> max_width,
    std::optional<double> max_height,
    double* min_width,
    double* min_height,
    std::vector<Rectangle>* item_boxes) {
  /* convert units  */
  legend_normalize(config, layer);

  /* remove padding */
  if (max_width) {
    max_width = *max_width - (config->padding[1] + config->padding[3]);
  }

  if (max_height) {
    max_height = *max_width - (config->padding[0] + config->padding[2]);
  }

  /* add item extents */
  ReturnCode layout_rc;
  if (config->item_flow) {
    layout_rc =
        legend_layout_item_flow(
            *config,
            layer,
            max_width,
            max_height,
            min_width,
            min_height,
            item_boxes);
  } else {
    layout_rc =
        legend_layout_item_rows(
            *config,
            layer,
            max_width,
            max_height,
            min_width,
            min_height,
            item_boxes);
  }

  if (!layout_rc) {
    return layout_rc;
  }

  /* add padding */
  *min_width += config->padding[1];
  *min_width += config->padding[3];
  *min_height += config->padding[0];
  *min_height += config->padding[2];

  return OK;
}

ReturnCode legend_draw_borders(
    const StrokeStyle* borders,
    const Rectangle& bbox,
    Page* layer) {
  /* draw top border  */
  if (borders[0].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = borders[0].line_width;
    border_style.color = borders[0].color;

    strokeLine(
        layer,
        Point(bbox.x, bbox.y),
        Point(bbox.x + bbox.w, bbox.y),
        border_style);
  }

  /* draw right border  */
  if (borders[1].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = borders[1].line_width;
    border_style.color = borders[1].color;

    strokeLine(
        layer,
        Point(bbox.x + bbox.w, bbox.y),
        Point(bbox.x + bbox.w, bbox.y + bbox.h),
        border_style);
  }

  /* draw top border  */
  if (borders[2].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = borders[2].line_width;
    border_style.color = borders[2].color;

    strokeLine(
        layer,
        Point(bbox.x, bbox.y + bbox.h),
        Point(bbox.x + bbox.w, bbox.y + bbox.h),
        border_style);
  }

  /* draw left border  */
  if (borders[3].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = borders[3].line_width;
    border_style.color = borders[3].color;

    strokeLine(
        layer,
        Point(bbox.x, bbox.y),
        Point(bbox.x, bbox.y + bbox.h),
        border_style);
  }
  return OK;
}

ReturnCode legend_draw_items(
    const LegendConfig& config,
    const Rectangle& bbox,
    const std::vector<Rectangle>& item_boxes,
    Page* layer) {
  for (size_t i = 0; i < std::min(config.items.size(), item_boxes.size()); ++i) {
    LayoutInfo layout;
    layout.content_box.x = bbox.x + item_boxes[i].x;
    layout.content_box.y = bbox.y + bbox.h - item_boxes[i].y - item_boxes[i].h;
    layout.content_box.w = item_boxes[i].w;
    layout.content_box.h = item_boxes[i].h;

    if (auto rc = config.items[i]->draw(layout, layer); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode legend_draw(
    std::shared_ptr<LegendConfig> config,
    const LayoutInfo& layout,
    Page* layer) {
  /* convert units  */
  legend_normalize(config, *layer);

  /* calculate boxes */
  auto parent_box = layout_margin_box(
      layout.content_box,
      config->margins[0],
      config->margins[1],
      config->margins[2],
      config->margins[3]);

  Rectangle border_box;
  std::vector<Rectangle> item_boxes;
  {
    auto pw = parent_box.w;
    auto ph = parent_box.h;
    double ew = 0;
    double eh = 0;
    if (auto rc = legend_layout(config, *layer, pw, ph, &ew, &eh, &item_boxes); !rc) {
      return rc;
    }

    border_box.h = std::min(eh, parent_box.h);
    border_box.w = std::min(ew, parent_box.w);

    switch (config->position_horiz) {
      case HAlign::LEFT:
        border_box.x = parent_box.x;
        break;
      case HAlign::CENTER:
        border_box.x = parent_box.x + parent_box.w * 0.5 - border_box.w * 0.5;
        break;
      case HAlign::RIGHT:
        border_box.x = parent_box.x + parent_box.w - border_box.w;
        break;
    }

    switch (config->position_vert) {
      case VAlign::TOP:
        border_box.y = parent_box.y + parent_box.h - border_box.h;
        break;
      case VAlign::CENTER:
        border_box.y = parent_box.y + parent_box.h * 0.5 - border_box.h * 0.5;
        break;
      case VAlign::BOTTOM:
        border_box.y = parent_box.y;
        break;
    }
  }

  auto content_box = layout_margin_box(
      border_box,
      config->padding[0],
      config->padding[1],
      config->padding[2],
      config->padding[3]);

  /* draw background */
  if (config->background) {
    const auto& bg_box = border_box;

    fillRectangle(
        layer,
        Point(bg_box.x, bg_box.y),
        bg_box.w,
        bg_box.h,
        *config->background);
  }

  /* draw borders */
  if (auto rc = legend_draw_borders(config->borders.data(), border_box, layer); !rc) {
    return rc;
  }

  /* draw items */
  if (auto rc = legend_draw_items(*config, content_box, item_boxes, layer); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode legend_configure_position(
    const Expr* expr,
    HAlign* position_horiz,
    VAlign* position_vert) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list but got: {}",
        expr_inspect(expr));
  }

  bool position_horiz_set = false;
  bool position_vert_set = false;

  for (expr = expr_get_list(expr); expr; expr = expr_next(expr)) {
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

ReturnCode configure_item(
    const std::string& item_elem_name,
    const Expr* expr,
    std::vector<LegendItemConfig>* items) {
  LegendItemConfig item;
  item.elem_name = item_elem_name;
  item.opts = expr_clone(expr_get_list(expr));
  items->emplace_back(std::move(item));
  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* inherit defaults */
  auto config = std::make_shared<LegendConfig>();
  config->item_row_padding = from_em(.3);
  config->item_column_padding = from_em(1.4);
  config->margins = std::array<Measure, 4>{from_em(.6), from_em(.6), from_em(.6), from_em(.6)};
  config->padding = std::array<Measure, 4>{from_em(.6), from_em(1), from_em(.6), from_em(1)};
  for (size_t i = 0; i < 4; ++i) {
    config->borders[i].color = env.foreground_color;
  }

  /* parse exprerties */
  std::vector<LegendItemConfig> items;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {
      "position",
      bind(
          &legend_configure_position,
          _1,
          &config->position_horiz,
          &config->position_vert)
    },
    {"item-row-padding", bind(&measure_read, _1, &config->item_row_padding)},
    {"item-column-padding", bind(&measure_read, _1, &config->item_column_padding)},
    {"item-flow", bind(&expr_to_switch, _1, &config->item_flow)},
    {"item", bind(&configure_item, "legend/item", _1, &items)},
    {"extra", bind(&element_build_list, env, _1, &config->items)},
    {
      "padding",
      expr_calln_fn({
        bind(&measure_read, _1, &config->padding[0]),
        bind(&measure_read, _1, &config->padding[1]),
        bind(&measure_read, _1, &config->padding[2]),
        bind(&measure_read, _1, &config->padding[3]),
      })
    },
    {"padding-top", bind(&measure_read, _1, &config->padding[0])},
    {"padding-right", bind(&measure_read, _1, &config->padding[1])},
    {"padding-bottom", bind(&measure_read, _1, &config->padding[2])},
    {"padding-left", bind(&measure_read, _1, &config->padding[3])},
    {
      "margin",
      expr_calln_fn({
        bind(&measure_read, _1, &config->margins[0]),
        bind(&measure_read, _1, &config->margins[1]),
        bind(&measure_read, _1, &config->margins[2]),
        bind(&measure_read, _1, &config->margins[3]),
      })
    },
    {"margin-top", bind(&measure_read, _1, &config->margins[0])},
    {"margin-right", bind(&measure_read, _1, &config->margins[1])},
    {"margin-bottom", bind(&measure_read, _1, &config->margins[2])},
    {"margin-left", bind(&measure_read, _1, &config->margins[3])},
    {
      "border",
      expr_calln_fn({
        bind(&expr_to_stroke_style, _1, &config->borders[0]),
        bind(&expr_to_stroke_style, _1, &config->borders[1]),
        bind(&expr_to_stroke_style, _1, &config->borders[2]),
        bind(&expr_to_stroke_style, _1, &config->borders[3])
      })
    },
    {
      "border-color",
      expr_calln_fn({
        bind(&color_read, env, _1, &config->borders[0].color),
        bind(&color_read, env, _1, &config->borders[1].color),
        bind(&color_read, env, _1, &config->borders[2].color),
        bind(&color_read, env, _1, &config->borders[3].color),
      })
    },
    {"border-top-color", bind(&color_read, env, _1, &config->borders[0].color)},
    {"border-right-color", bind(&color_read, env, _1, &config->borders[1].color)},
    {"border-bottom-color", bind(&color_read, env, _1, &config->borders[2].color)},
    {"border-left-color", bind(&color_read, env, _1, &config->borders[3].color)},
    {
      "border-width",
      expr_calln_fn({
        bind(&measure_read, _1, &config->borders[0].line_width),
        bind(&measure_read, _1, &config->borders[1].line_width),
        bind(&measure_read, _1, &config->borders[2].line_width),
        bind(&measure_read, _1, &config->borders[3].line_width),
      })
    },
    {"border-top-width", bind(&measure_read, _1, &config->borders[0].line_width)},
    {"border-right-width", bind(&measure_read, _1, &config->borders[1].line_width)},
    {"border-bottom-width", bind(&measure_read, _1, &config->borders[2].line_width)},
    {"border-left-width", bind(&measure_read, _1, &config->borders[3].line_width)},
    {"background", bind(&color_read_opt, env, _1, &config->background)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* build the items */
  for (const auto& item : items) {
    auto elem_config = expr_build(
        item.elem_name,
        expr_clone(item.opts.get()));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->items.emplace_back(elem);
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&legend_draw, config, _1, _2);
  (*elem)->size_hint = bind(&legend_layout, config, _1, _2, _3, _4, _5, nullptr);
  return OK;
}

} // namespace fviz::elements::legend


/**
 * This file is part of the "clip" project
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
#include "legend_item.h"

#include "data.h"
#include "context.h"
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

namespace clip::plotgen {

struct LegendConfig {
  LegendConfig();
  HAlign position_horiz;
  VAlign position_vert;
  Number item_row_padding;
  Number item_column_padding;
  bool item_flow;
  std::array<Number, 4> margins;
  std::array<Number, 4> padding;
  std::array<StrokeStyle, 4> borders;
  Option<Color> background;
  std::vector<LegendItem> items;
};

LegendConfig::LegendConfig() :
    position_horiz(HAlign::LEFT),
    position_vert(VAlign::TOP),
    item_flow(false) {}

ReturnCode legend_layout_item_rows(
    Context* ctx,
    LegendConfig* config,
    const Option<double> max_width,
    const Option<double> max_height,
    double* min_width,
    double* min_height,
    std::vector<Rectangle>* item_boxes) {
  /* calculate vertical size */
  double m_width = 0;
  double m_height = 0;

  for (auto& e : config->items) {
    auto p_width = max_width;
    auto p_height = max_height;
    if (p_height) {
      p_height = *p_height - m_height;
    }

    double e_width = 0;
    double e_height = 0;
    if (auto rc = legend_item_calculate_size(ctx, &e, &e_width, &e_height); !rc) {
      return rc;
    }

    if (m_height > 0) {
      m_height += config->item_row_padding.value;
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
    Context* ctx,
    LegendConfig* config,
    const Option<double> max_width,
    const Option<double> max_height,
    double* min_width,
    double* min_height,
    std::vector<Rectangle>* item_boxes) {
  /* calculate vertical size */
  double m_width = 0;
  double m_height = 0;

  size_t r_begin = item_boxes ? item_boxes->size() : 0;
  double r_width = 0;
  double r_height = 0;
  for (auto& e : config->items) {
    double e_width = 0;
    double e_height = 0;
    if (auto rc = legend_item_calculate_size(ctx, &e, &e_width, &e_height); !rc) {
      return rc;
    }

    if (r_width > 0) {
      r_width += config->item_column_padding.value;
    }

    if (max_width && r_width + e_width > *max_width) {
      m_height += r_height + config->item_row_padding.value;
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
    Context* ctx,
    LegendConfig* config,
    Option<double> max_width,
    Option<double> max_height,
    double* min_width,
    double* min_height,
    std::vector<Rectangle>* item_boxes) {
  /* remove padding */
  if (max_width) {
    max_width = *max_width - (config->padding[1].value + config->padding[3].value);
  }

  if (max_height) {
    max_height = *max_width - (config->padding[0].value + config->padding[2].value);
  }

  /* add item extents */
  ReturnCode layout_rc;
  if (config->item_flow) {
    layout_rc =
        legend_layout_item_flow(
            ctx,
            config,
            max_width,
            max_height,
            min_width,
            min_height,
            item_boxes);
  } else {
    layout_rc =
        legend_layout_item_rows(
            ctx,
            config,
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
  *min_width += config->padding[1].value;
  *min_width += config->padding[3].value;
  *min_height += config->padding[0].value;
  *min_height += config->padding[2].value;

  return OK;
}

ReturnCode plot_legend_borders(
    Context* ctx,
    const StrokeStyle* borders,
    const Rectangle& bbox) {
  /* draw top border  */
  if (borders[0].line_width.value > 0) {
    DrawCommand line;
    line.stroke_style.line_width = borders[0].line_width;
    line.stroke_style.color = borders[0].color;

    path_add_line(
        &line.path,
        Point(bbox.x, bbox.y),
        Point(bbox.x + bbox.w, bbox.y));

    draw_shape(ctx, line);
  }

  /* draw right border  */
  if (borders[1].line_width.value > 0) {
    DrawCommand line;
    line.stroke_style.line_width = borders[1].line_width;
    line.stroke_style.color = borders[1].color;

    path_add_line(
        &line.path,
        Point(bbox.x + bbox.w, bbox.y),
        Point(bbox.x + bbox.w, bbox.y + bbox.h));

    draw_shape(ctx, line);
  }

  /* draw top border  */
  if (borders[2].line_width.value > 0) {
    DrawCommand line;
    line.stroke_style.line_width = borders[2].line_width;
    line.stroke_style.color = borders[2].color;

    path_add_line(
        &line.path,
        Point(bbox.x, bbox.y + bbox.h),
        Point(bbox.x + bbox.w, bbox.y + bbox.h));

    draw_shape(ctx, line);
  }

  /* draw left border  */
  if (borders[3].line_width.value > 0) {
    DrawCommand line;
    line.stroke_style.line_width = borders[3].line_width;
    line.stroke_style.color = borders[3].color;

    path_add_line(
        &line.path,
        Point(bbox.x, bbox.y),
        Point(bbox.x, bbox.y + bbox.h));

    draw_shape(ctx, line);
  }

  return OK;
}

ReturnCode plot_legend_items(
    Context* ctx,
    LegendConfig* config,
    const Rectangle& bbox,
    const std::vector<Rectangle>& item_boxes) {
  for (size_t i = 0; i < std::min(config->items.size(), item_boxes.size()); ++i) {
    Rectangle item_bbox;
    item_bbox.x = bbox.x + item_boxes[i].x;
    item_bbox.y = bbox.y + bbox.h - item_boxes[i].y - item_boxes[i].h;
    item_bbox.w = item_boxes[i].w;
    item_bbox.h = item_boxes[i].h;

    if (auto rc = legend_item_draw(ctx, &config->items[i], item_bbox); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode plot_legend(
    Context* ctx,
    PlotConfig* plot,
    LegendConfig* config) {
  /* calculate boxes */
  const auto& bbox = plot_get_clip(plot, layer_get(ctx));
  auto parent_box = layout_margin_box(
      bbox,
      config->margins[0].value,
      config->margins[1].value,
      config->margins[2].value,
      config->margins[3].value);

  Rectangle border_box;
  std::vector<Rectangle> item_boxes;
  {
    auto pw = parent_box.w;
    auto ph = parent_box.h;
    double ew = 0;
    double eh = 0;
    if (auto rc = legend_layout(ctx, config, pw, ph, &ew, &eh, &item_boxes); !rc) {
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
      config->padding[0].value,
      config->padding[1].value,
      config->padding[2].value,
      config->padding[3].value);

  /* draw background */
  if (config->background) {
    DrawCommand shape;
    shape.fill_style.color = *config->background;
    path_add_rectangle(&shape.path, border_box);
    draw_shape(ctx, shape);
  }

  /* draw borders */
  if (auto rc = plot_legend_borders(
        ctx,
        config->borders.data(),
        border_box);
      !rc) {
    return rc;
  }

  /* draw items */
  if (auto rc = plot_legend_items(
        ctx,
        config,
        content_box,
        item_boxes);
      !rc) {
    return rc;
  }

  return OK;
}

ReturnCode legend_configure_position(
    const Expr* expr,
    HAlign* position_horiz,
    VAlign* position_vert) {
  bool position_horiz_set = false;
  bool position_vert_set = false;

  for (; expr; expr = expr_next(expr)) {
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
    Context* ctx,
    const Expr* expr,
    std::vector<LegendItem>* items) {
  LegendItem item;
  if (auto rc = legend_item_configure(ctx, expr, &item); !rc) {
    return rc;
  }

  items->push_back(std::move(item));
  return OK;
}

ReturnCode plot_legend(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* inherit defaults */
  auto config = std::make_shared<LegendConfig>();
  config->item_row_padding = unit_from_em(.3, layer_get_font_size(layer));
  config->item_column_padding = unit_from_em(1.4, layer_get_font_size(layer));
  config->margins = std::array<Number, 4>{
    unit_from_em(.6, layer_get_font_size(layer)),
    unit_from_em(.6, layer_get_font_size(layer)),
    unit_from_em(.6, layer_get_font_size(layer)),
    unit_from_em(.6, layer_get_font_size(layer))
  };

  config->padding = std::array<Number, 4>{
    unit_from_em(.6, layer_get_font_size(layer)),
    unit_from_em(1, layer_get_font_size(layer)),
    unit_from_em(.6, layer_get_font_size(layer)),
    unit_from_em(1, layer_get_font_size(layer))
  };

  for (size_t i = 0; i < 4; ++i) {
    config->borders[i].color = layer_get(ctx)->foreground_color;
  }

  /* parse exprerties */
  auto config_rc = expr_walk_map(expr, {
    {
      "position",
      std::bind(
          &legend_configure_position,
          _1,
          &config->position_horiz,
          &config->position_vert)
    },
    {"item-row-padding", std::bind(&expr_to_size, _1, layer, &config->item_row_padding)},
    {"item-column-padding", std::bind(&expr_to_size, _1, layer, &config->item_column_padding)},
    {"item-flow", std::bind(&expr_to_switch, _1, &config->item_flow)},
    {"item", std::bind(&configure_item, ctx, _1, &config->items)},
    {
      "padding",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &config->padding[0]),
        std::bind(&expr_to_size, _1, layer, &config->padding[1]),
        std::bind(&expr_to_size, _1, layer, &config->padding[2]),
        std::bind(&expr_to_size, _1, layer, &config->padding[3]),
      })
    },
    {"padding-top", std::bind(&expr_to_size, _1, layer, &config->padding[0])},
    {"padding-right", std::bind(&expr_to_size, _1, layer, &config->padding[1])},
    {"padding-bottom", std::bind(&expr_to_size, _1, layer, &config->padding[2])},
    {"padding-left", std::bind(&expr_to_size, _1, layer, &config->padding[3])},
    {
      "margin",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &config->margins[0]),
        std::bind(&expr_to_size, _1, layer, &config->margins[1]),
        std::bind(&expr_to_size, _1, layer, &config->margins[2]),
        std::bind(&expr_to_size, _1, layer, &config->margins[3]),
      })
    },
    {"margin-top", std::bind(&expr_to_size, _1, layer, &config->margins[0])},
    {"margin-right", std::bind(&expr_to_size, _1, layer, &config->margins[1])},
    {"margin-bottom", std::bind(&expr_to_size, _1, layer, &config->margins[2])},
    {"margin-left", std::bind(&expr_to_size, _1, layer, &config->margins[3])},
    {
      "border",
      expr_calln_fn({
        std::bind(&expr_to_stroke_style, _1, &config->borders[0]),
        std::bind(&expr_to_stroke_style, _1, &config->borders[1]),
        std::bind(&expr_to_stroke_style, _1, &config->borders[2]),
        std::bind(&expr_to_stroke_style, _1, &config->borders[3])
      })
    },
    {
      "border-color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &config->borders[0].color),
        std::bind(&color_read, ctx, _1, &config->borders[1].color),
        std::bind(&color_read, ctx, _1, &config->borders[2].color),
        std::bind(&color_read, ctx, _1, &config->borders[3].color),
      })
    },
    {"border-top-color", std::bind(&color_read, ctx, _1, &config->borders[0].color)},
    {"border-right-color", std::bind(&color_read, ctx, _1, &config->borders[1].color)},
    {"border-bottom-color", std::bind(&color_read, ctx, _1, &config->borders[2].color)},
    {"border-left-color", std::bind(&color_read, ctx, _1, &config->borders[3].color)},
    {
      "border-width",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &config->borders[0].line_width),
        std::bind(&expr_to_size, _1, layer, &config->borders[1].line_width),
        std::bind(&expr_to_size, _1, layer, &config->borders[2].line_width),
        std::bind(&expr_to_size, _1, layer, &config->borders[3].line_width),
      })
    },
    {"border-top-width", std::bind(&expr_to_size, _1, layer, &config->borders[0].line_width)},
    {"border-right-width", std::bind(&expr_to_size, _1, layer, &config->borders[1].line_width)},
    {"border-bottom-width", std::bind(&expr_to_size, _1, layer, &config->borders[2].line_width)},
    {"border-left-width", std::bind(&expr_to_size, _1, layer, &config->borders[3].line_width)},
    {"background", std::bind(&color_read_opt, ctx, _1, &config->background)},
  });

  if (!config_rc) {
    return config_rc;
  }

  return plot_legend(ctx, plot, config.get());
}

} // namespace clip::plotgen


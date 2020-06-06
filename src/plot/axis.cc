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
#include "axis.h"

#include "data.h"
#include "context.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "graphics/brush.h"
#include "scale.h"
#include "format.h"
#include "layout.h"
#include "color_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <assert.h>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

static const double kDefaultTitlePaddingHorizEM = .6;
static const double kDefaultTitlePaddingVertEM = .8;
static const double kDefaultLabelPaddingEM = 0.8;
static const double kDefaultLineWidthPT = 1;
static const double kDefaultTickLengthPT = 4;

enum class AxisAlign {X, Y, TOP, RIGHT, BOTTOM, LEFT};

enum class AxisLabelAttach {TOP, RIGHT, BOTTOM, LEFT};

struct AxisDefinition;

using AxisLabelPlacement = std::function<ReturnCode (
    const ScaleConfig& domain,
    AxisDefinition*)>;

struct AxisDefinition {
  AxisDefinition();

  /* layout */
  AxisAlign align;
  bool enable;

  /* scale */
  ScaleConfig scale;

  /* title */
  std::string title;
  Color title_color;
  double title_offset;
  Measure title_padding;
  FontInfo title_font;
  Number title_font_size;
  double title_rotate;

  /* ticks */
  double tick_offset;
  Measure tick_length;
  ScaleLayoutFn tick_placement;

  /* labels */
  ScaleLayoutFn label_placement;
  Formatter label_formatter;
  AxisLabelAttach label_attach;
  double label_offset;
  Measure label_padding;
  double label_rotate;
  FontInfo label_font;
  Number label_font_size;
  Color label_color;

  /* border */
  StrokeStyle border_style;
};

AxisDefinition::AxisDefinition() :
    align(AxisAlign::X),
    enable(true),
    title_offset(0.0),
    title_rotate(0.0),
    tick_offset(0.0),
    label_offset(0.0),
    label_rotate(0.0) {}

ReturnCode axis_layout_labels(
    const AxisDefinition& axis,
    const Context* ctx,
    double* margin) {
  /* compute scale layout */
  ScaleLayout labels;
  axis.label_placement(axis.scale, axis.label_formatter, &labels);

  /* compute label margin */
  double label_margin = 0;
  for (size_t i = 0; i < labels.positions.size(); ++i) {
    auto tick = labels.positions[i];
    auto label_text = labels.labels[i];

    TextStyle style;
    style.font = axis.label_font;
    style.font_size = axis.label_font_size;

    Rectangle label_bbox;
    if (auto rc = text_measure_label(
          label_text,
          TextDirection::LTR,
          axis.label_font,
          axis.label_font_size,
          layer_get_dpi(ctx),
          &label_bbox);
         rc != Status::OK) {
      return rc;
    }

    if (axis.label_rotate) {
      label_bbox = box_rotate_bounds(label_bbox, axis.label_rotate);
    }

    switch (axis.align) {
      case AxisAlign::X:
      case AxisAlign::TOP:
      case AxisAlign::BOTTOM:
        label_margin = std::max(label_margin, label_bbox.h);
        break;
        break;
      case AxisAlign::Y:
      case AxisAlign::LEFT:
      case AxisAlign::RIGHT:
        label_margin = std::max(label_margin, label_bbox.w);
        break;
    }
  }

  *margin += label_margin;
  return OK;
}

ReturnCode axis_layout_title(
    const AxisDefinition& axis,
    const Context* ctx,
    double* margin) {
  Rectangle title_bbox;
  if (auto rc = text_measure_label(
        axis.title,
        TextDirection::LTR,
        axis.title_font,
        axis.title_font_size,
        layer_get_dpi(ctx),
        &title_bbox);
       rc != Status::OK) {
    return rc;
  }

  if (axis.title_rotate) {
    title_bbox = box_rotate_bounds(title_bbox, axis.title_rotate);
  }

  switch (axis.align) {
    case AxisAlign::X:
    case AxisAlign::LEFT:
    case AxisAlign::RIGHT:
      *margin += title_bbox.w;
      break;
    case AxisAlign::Y:
    case AxisAlign::TOP:
    case AxisAlign::BOTTOM:
      *margin += title_bbox.h;
      break;
      break;
  }

  return OK;
}

ReturnCode axis_layout(
    const AxisDefinition& axis,
    const Context* ctx,
    double* margin) {

  /* add margin for the labels */
  {
    *margin += measure_or(
        axis.label_padding,
        from_em(kDefaultLabelPaddingEM, axis.label_font_size.value));

    if (auto rc = axis_layout_labels(axis, ctx, margin); !rc) {
      return rc;
    }
  }

  /* add margin for the title */
  if (!axis.title.empty()) {
    switch (axis.align) {
      case AxisAlign::X:
      case AxisAlign::TOP:
      case AxisAlign::BOTTOM:
        *margin += measure_or(
                axis.title_padding,
                from_em(kDefaultTitlePaddingVertEM, axis.title_font_size.value));
        break;
      case AxisAlign::Y:
      case AxisAlign::LEFT:
      case AxisAlign::RIGHT:
        *margin += measure_or(
                axis.title_padding,
                from_em(kDefaultTitlePaddingHorizEM, axis.title_font_size.value));
        break;
    }

    if (auto rc = axis_layout_title(axis, ctx, margin); !rc) {
      return rc;
    }
  }

  return OK;
}

static ReturnCode plot_axis_vertical(
    Context* ctx,
    PlotConfig* plot,
    const AxisDefinition& axis_config,
    double x,
    double y0,
    double y1) {
  /* draw axis line */
  draw_line(ctx, {x, y0}, {x, y1}, axis_config.border_style);

  /* draw ticks */
  ScaleLayout ticks;
  axis_config.tick_placement(
      axis_config.scale,
      axis_config.label_formatter,
      &ticks);

  double tick_position =
      std::clamp(axis_config.tick_offset, -1.0, 1.0);

  auto tick_length = measure_or(
      axis_config.tick_length,
      from_pt(kDefaultTickLengthPT, layer_get_dpi(ctx)));

  for (const auto& tick : ticks.positions) {
    auto ty = y0 + (y1 - y0) * tick;
    auto tx = x - tick_length + tick_length * (tick_position + 1) / 2.0;

    draw_line(
        ctx,
        {tx, ty},
        {tx + tick_length, ty},
        axis_config.border_style);
  }

  /* draw labels */
  ScaleLayout labels;
  axis_config.label_placement(
      axis_config.scale,
      axis_config.label_formatter,
      &labels);

  double label_position =
      std::clamp(axis_config.label_offset, -1.0, 1.0);

  auto label_padding = measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, axis_config.label_font_size.value));

  double label_size = 0;
  if (auto rc = axis_layout_labels(axis_config, ctx, &label_size);
      !rc) {
    return rc;
  }

  for (size_t i = 0; i < labels.positions.size(); ++i) {
    auto tick = labels.positions[i];
    auto label_text = labels.labels[i];

    Point p;
    p.x = x + label_padding * label_position;
    p.y = y0 + (y1 - y0) * tick;

    TextStyle style;
    style.font = axis_config.label_font;
    style.color = axis_config.label_color;
    style.font_size = axis_config.label_font_size;

    auto a = axis_config.label_rotate;
    HAlign ax;
    VAlign ay;
    switch (axis_config.label_attach) {
      case AxisLabelAttach::LEFT:
        a = axis_config.label_rotate;
        ax = HAlign::LEFT;
        ay = VAlign::CENTER;
        break;
      case AxisLabelAttach::TOP:
        ax = HAlign::CENTER;
        ay = VAlign::TOP;
        break;
      case AxisLabelAttach::BOTTOM:
        ax = HAlign::CENTER;
        ay = VAlign::BOTTOM;
        break;
      case AxisLabelAttach::RIGHT:
        ax = HAlign::RIGHT;
        ay = VAlign::CENTER;
        break;
    }

    if (auto rc = draw_text(ctx, label_text, p, ax, ay, a, style); rc != OK) {
      return rc;
    }
  }

  /* draw title */
  if (!axis_config.title.empty()) {
    double title_position =
        std::clamp(axis_config.title_offset, -1.0, 1.0);

    double title_padding = measure_or(
        axis_config.title_padding,
        from_em(kDefaultTitlePaddingHorizEM, axis_config.title_font_size.value));

    if ((title_position < 0) == (label_position < 0)) {
      title_padding += label_size + label_padding;
    }

    double title_size = 0;
    if (auto rc =
          axis_layout_title(axis_config, ctx, &title_size);
        !rc) {
      return rc;
    }

    Point p;
    p.x = x + (title_padding + title_size * 0.5) * title_position;
    p.y = y0 + (y1 - y0) * 0.5;

    TextStyle style;
    style.font = axis_config.title_font;
    style.color = axis_config.title_color;
    style.font_size = axis_config.title_font_size;

    auto draw_rc =
      draw_text(
          ctx,
          axis_config.title,
          p,
          HAlign::CENTER,
          VAlign::CENTER,
          axis_config.title_rotate,
          style);

    if (!draw_rc) {
      return draw_rc;
    }
  }

  return OK;
}

static ReturnCode plot_axis_horizontal(
    Context* ctx,
    PlotConfig* plot,
    const AxisDefinition& axis_config,
    double y,
    double x0,
    double x1) {
  /* draw axis line */
  draw_line(ctx, {x0, y}, {x1, y}, axis_config.border_style);

  /* draw ticks */
  ScaleLayout ticks;
  axis_config.tick_placement(
      axis_config.scale,
      axis_config.label_formatter,
      &ticks);

  double tick_position =
      std::clamp(axis_config.tick_offset, -1.0, 1.0);

  auto tick_length = measure_or(
      axis_config.tick_length,
      from_pt(kDefaultTickLengthPT, layer_get_dpi(ctx)));

  for (const auto& tick : ticks.positions) {
    auto ty = y - tick_length + tick_length * (tick_position + 1) / 2.0;
    auto tx = x0 + (x1 - x0) * tick;
    draw_line(
        ctx,
        {tx, ty},
        {tx, ty + tick_length},
        axis_config.border_style);
  }

  /* draw labels */
  ScaleLayout labels;
  axis_config.label_placement(
      axis_config.scale,
      axis_config.label_formatter,
      &labels);

  double label_position =
      std::clamp(axis_config.label_offset, -1.0, 1.0);

  double label_padding =  measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, axis_config.label_font_size.value));

  double label_size = 0;
  if (auto rc = axis_layout_labels(axis_config, ctx, &label_size); !rc) {
    return rc;
  }

  for (size_t i = 0; i < labels.positions.size(); ++i) {
    auto tick = labels.positions[i];
    auto label_text = labels.labels[i];

    Point p;
    p.x = x0 + (x1 - x0) * tick;
    p.y = y + label_padding * label_position;

    TextStyle style;
    style.font = axis_config.label_font;
    style.color = axis_config.label_color;
    style.font_size = axis_config.label_font_size;

    auto a = axis_config.label_rotate;
    HAlign ax;
    VAlign ay;
    switch (axis_config.label_attach) {
      case AxisLabelAttach::LEFT:
        a = axis_config.label_rotate;
        ax = HAlign::LEFT;
        ay = VAlign::CENTER;
        break;
      case AxisLabelAttach::TOP:
        ax = HAlign::CENTER;
        ay = VAlign::TOP;
        break;
      case AxisLabelAttach::BOTTOM:
        ax = HAlign::CENTER;
        ay = VAlign::BOTTOM;
        break;
      case AxisLabelAttach::RIGHT:
        ax = HAlign::RIGHT;
        ay = VAlign::CENTER;
        break;
    }

    if (auto rc = draw_text(ctx, label_text, p, ax, ay, a, style); !rc) {
      return rc;
    }
  }

  /* draw title */
  if (!axis_config.title.empty()) {
    double title_position =
        std::clamp(axis_config.title_offset, -1.0, 1.0);

    double title_padding = measure_or(
        axis_config.title_padding,
        from_em(kDefaultTitlePaddingVertEM, axis_config.title_font_size.value));

    if ((title_position < 0) == (label_position < 0)) {
      title_padding += label_size + label_padding;
    }

    double title_size = 0;
    if (auto rc = axis_layout_title(axis_config, ctx, &title_size); !rc) {
      return rc;
    }

    Point p;
    p.x = x0 + (x1 - x0) * 0.5;
    p.y = y + (title_padding + title_size * 0.5) * title_position;

    TextStyle style;
    style.font = axis_config.title_font;
    style.color = axis_config.title_color;
    style.font_size = axis_config.title_font_size;

    auto draw_rc = draw_text(
        ctx,
        axis_config.title,
        p,
        HAlign::CENTER,
        VAlign::CENTER,
        axis_config.title_rotate,
        style);

    if (!draw_rc) {
      return draw_rc;
    }
  }

  return OK;
}

ReturnCode axis_set_defaults(
    Context* ctx,
    AxisDefinition* config) {
  scale_configure_layout_defaults(
      config->scale,
      &config->label_placement,
      &config->tick_placement);

  if (!config->label_formatter) {
    if (config->scale.kind == ScaleKind::CATEGORICAL) {
      config->label_formatter = format_noop();
    } else {
      config->label_formatter = format_decimal_fixed(1);
    }
  }

  return OK;
}

ReturnCode plot_axis(
    Context* ctx,
    PlotConfig* plot,
    AxisDefinition* config) {
  const auto& axis = *config;
  const auto& bbox = plot_get_clip(plot, layer_get(ctx));

  ReturnCode rc;
  switch (axis.align) {
    case AxisAlign::X:
      rc = plot_axis_horizontal(
          ctx,
          plot,
          axis,
          bbox.y + bbox.h * 0.5,
          bbox.x,
          bbox.x + bbox.w);
      break;
    case AxisAlign::TOP:
      rc = plot_axis_horizontal(
          ctx,
          plot,
          axis,
          bbox.y + bbox.h,
          bbox.x,
          bbox.x + bbox.w);
      break;
    case AxisAlign::BOTTOM:
      rc = plot_axis_horizontal(
          ctx,
          plot,
          axis,
          bbox.y,
          bbox.x,
          bbox.x + bbox.w);
      break;
    case AxisAlign::Y:
      rc = plot_axis_vertical(
          ctx,
          plot,
          axis,
          bbox.x + bbox.w * 0.5,
          bbox.y,
          bbox.y + bbox.h);
      break;
    case AxisAlign::LEFT:
      rc = plot_axis_vertical(
          ctx,
          plot,
          axis,
          bbox.x,
          bbox.y,
          bbox.y + bbox.h);
      break;
    case AxisAlign::RIGHT:
      rc = plot_axis_vertical(
          ctx,
          plot,
          axis,
          bbox.x + bbox.w,
          bbox.y,
          bbox.y + bbox.h);
      break;
  }

  return rc;
}

ReturnCode plot_axis(Context* ctx, PlotConfig* plot, const Expr* expr) {
  const auto& layer = *layer_get(ctx);
  auto elem_name = expr_get_value(expr);

  auto config = std::make_shared<AxisDefinition>();
  config->label_font = layer_get_font(ctx);
  config->label_font_size = layer_get_font_size(ctx);
  config->label_color = layer_get(ctx)->text_color;
  config->title_font = layer_get_font(ctx);
  config->title_font_size = layer_get_font_size(ctx);
  config->title_color = layer_get(ctx)->text_color;
  config->border_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));
  config->border_style.color = layer_get(ctx)->foreground_color;

  {
    auto rc = expr_walk_map(expr, {
      {
        "align",
        expr_to_enum_fn<AxisAlign>(&config->align, {
          {"x", AxisAlign::X},
          {"y", AxisAlign::Y},
          {"top", AxisAlign::TOP},
          {"right", AxisAlign::RIGHT},
          {"bottom", AxisAlign::BOTTOM},
          {"left", AxisAlign::LEFT},
        })
      }
    }, false);

    if (!rc) {
      return rc;
    }
  }

  switch (config->align) {
    case AxisAlign::X:
      config->label_attach = AxisLabelAttach::TOP;
      config->title_offset = -1;
      config->tick_offset = 0;
      config->label_offset = -1;
      break;
    case AxisAlign::TOP:
      config->label_attach = AxisLabelAttach::BOTTOM;
      config->title_offset = 1;
      config->tick_offset = -1;
      config->label_offset = 1;
      break;
    case AxisAlign::BOTTOM:
      config->label_attach = AxisLabelAttach::TOP;
      config->title_offset = -1;
      config->tick_offset = 1;
      config->label_offset = -1;
      break;
    case AxisAlign::Y:
      config->label_attach = AxisLabelAttach::RIGHT;
      config->title_offset = 1;
      config->title_rotate = -90;
      config->tick_offset = -1;
      config->label_offset = 1;
      break;
    case AxisAlign::LEFT:
      config->label_attach = AxisLabelAttach::RIGHT;
      config->title_offset = -1;
      config->title_rotate = -90;
      config->tick_offset = 1;
      config->label_offset = -1;
      break;
    case AxisAlign::RIGHT:
      config->label_attach = AxisLabelAttach::LEFT;
      config->title_offset = 1;
      config->title_rotate = -90;
      config->tick_offset = -1;
      config->label_offset = 1;
      break;
  };

  {
    auto rc = expr_walk_map(expr, {
      /* scale options */
      {"scale", std::bind(&scale_configure_kind, _1, &config->scale)},
      {"limit", std::bind(&expr_to_float64_opt_pair, _1, &config->scale.min, &config->scale.max)},
      {"limit-min", std::bind(&expr_to_float64_opt, _1, &config->scale.min)},
      {"limit-max", std::bind(&expr_to_float64_opt, _1, &config->scale.max)},

      /* label options */
      {"label-font", expr_call_string_fn(std::bind(&font_load_best, _1, &config->label_font))},
      {"label-placement", std::bind(&scale_configure_layout, _1, &config->label_placement)},
      {"label-format", std::bind(&format_configure, _1, &config->label_formatter)},
      {
        "label-attach",
        expr_to_enum_fn<AxisLabelAttach>(&config->label_attach, {
          { "top", AxisLabelAttach::TOP },
          { "right", AxisLabelAttach::RIGHT },
          { "bottom", AxisLabelAttach::BOTTOM },
          { "left", AxisLabelAttach::LEFT },
        })
      },
      {"label-offset", std::bind(&expr_to_float64, _1, &config->label_offset)},
      {"label-padding", std::bind(&measure_read, _1, &config->label_padding)},
      {"label-rotate", std::bind(&expr_to_float64, _1, &config->label_rotate)},
      {"label-font-size", std::bind(&expr_to_font_size, _1, layer, &config->label_font_size)},
      {"label-color", std::bind(&color_read, ctx, _1, &config->label_color)},

      /* tick options */
      {"tick-placement", std::bind(&scale_configure_layout, _1, &config->tick_placement)},
      {"tick-offset", std::bind(&expr_to_float64, _1, &config->tick_offset)},
      {"tick-length", std::bind(&measure_read, _1, &config->tick_length)},

      /* title options */
      {"title", std::bind(&expr_to_string, _1, &config->title)},
      {"title-font", expr_call_string_fn(std::bind(&font_load_best, _1, &config->title_font))},
      {"title-font-size", std::bind(&expr_to_font_size, _1, layer, &config->title_font_size)},
      {"title-color", std::bind(&color_read, ctx, _1, &config->title_color)},
      {"title-offset", std::bind(&expr_to_float64, _1, &config->title_offset)},
      {"title-padding", std::bind(&measure_read, _1, &config->title_padding)},
      {"title-rotate", std::bind(&expr_to_float64, _1, &config->title_rotate)},

      /* border options */
      {"border-width", std::bind(&expr_to_size, _1, layer, &config->border_style.line_width)},
      {"border-color", std::bind(&color_read, ctx, _1, &config->border_style.color)},
      {"border-style", std::bind(&expr_to_stroke_style, _1, &config->border_style)},

      /* global font options */
      {
        "font",
        expr_calln_fn({
          expr_call_string_fn(std::bind(&font_load_best, _1, &config->label_font)),
          expr_call_string_fn(std::bind(&font_load_best, _1, &config->title_font)),
        })
      },
      {
        "font-size",
        expr_calln_fn({
          std::bind(&expr_to_font_size, _1, layer, &config->label_font_size),
          std::bind(&expr_to_font_size, _1, layer, &config->title_font_size),
        })
      }
    }, false);

    if (!rc) {
      return rc;
    }
  }

  if (auto rc = axis_set_defaults(ctx, config.get()); !rc) {
    return rc;
  }

  return plot_axis(ctx, plot, config.get());
}

ReturnCode axis_configure_position(
    const Expr* expr,
    std::array<AxisDefinition, 4>* axes) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list but got: {}",
        expr_inspect(expr));
  }


  axes->at(0).enable = false;
  axes->at(1).enable = false;
  axes->at(2).enable = false;
  axes->at(3).enable = false;

  for (expr = expr_get_list(expr); expr; expr = expr_next(expr)) {
    if (expr_is_value(expr, "top")) {
      axes->at(0).enable = true;
      continue;
    }

    if (expr_is_value(expr, "bottom")) {
      axes->at(2).enable = true;
      continue;
    }

    if (expr_is_value(expr, "left")) {
      axes->at(3).enable = true;
      continue;
    }

    if (expr_is_value(expr, "right")) {
      axes->at(1).enable = true;
      continue;
    }

    return ERROR;
  }

  return OK;
}

ReturnCode plot_axes(Context* ctx, PlotConfig* plot, const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  std::array<Number, 4> margins = {
    unit_from_em(1, layer_get_font_size(layer)),
    unit_from_em(1, layer_get_font_size(layer)),
    unit_from_em(1, layer_get_font_size(layer)),
    unit_from_em(1, layer_get_font_size(layer))
  };

  std::array<AxisDefinition, 4> axes;

  axes[0].scale = plot->scale_x;
  axes[0].align = AxisAlign::TOP;
  axes[0].label_attach = AxisLabelAttach::BOTTOM;
  axes[0].title_offset = 1;
  axes[0].tick_offset = -1;
  axes[0].label_offset = 1;
  axes[0].label_font = layer_get_font(ctx);
  axes[0].label_font_size = layer_get_font_size(ctx);
  axes[0].label_color = layer_get(ctx)->text_color;
  axes[0].title_font = layer_get_font(ctx);
  axes[0].title_font_size = layer_get_font_size(ctx);
  axes[0].title_color = layer_get(ctx)->text_color;
  axes[0].border_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));
  axes[0].border_style.color = layer_get(ctx)->foreground_color;

  axes[1].scale = plot->scale_y;
  axes[1].align = AxisAlign::RIGHT;
  axes[1].label_attach = AxisLabelAttach::LEFT;
  axes[1].title_offset = 1;
  axes[1].title_rotate = -90;
  axes[1].tick_offset = -1;
  axes[1].label_offset = 1;
  axes[1].label_font = layer_get_font(ctx);
  axes[1].label_font_size = layer_get_font_size(ctx);
  axes[1].label_color = layer_get(ctx)->text_color;
  axes[1].title_font = layer_get_font(ctx);
  axes[1].title_font_size = layer_get_font_size(ctx);
  axes[1].title_color = layer_get(ctx)->text_color;
  axes[1].border_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));
  axes[1].border_style.color = layer_get(ctx)->foreground_color;

  axes[2].scale = plot->scale_x;
  axes[2].align = AxisAlign::BOTTOM;
  axes[2].label_attach = AxisLabelAttach::TOP;
  axes[2].title_offset = -1;
  axes[2].tick_offset = 1;
  axes[2].label_offset = -1;
  axes[2].label_font = layer_get_font(ctx);
  axes[2].label_font_size = layer_get_font_size(ctx);
  axes[2].label_color = layer_get(ctx)->text_color;
  axes[2].title_font = layer_get_font(ctx);
  axes[2].title_font_size = layer_get_font_size(ctx);
  axes[2].title_color = layer_get(ctx)->text_color;
  axes[2].border_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));
  axes[2].border_style.color = layer_get(ctx)->foreground_color;

  axes[3].scale = plot->scale_y;
  axes[3].align = AxisAlign::LEFT;
  axes[3].label_attach = AxisLabelAttach::RIGHT;
  axes[3].title_offset = -1;
  axes[3].title_rotate = -90;
  axes[3].tick_offset = 1;
  axes[3].label_offset = -1;
  axes[3].label_font = layer_get_font(ctx);
  axes[3].label_font_size = layer_get_font_size(ctx);
  axes[3].label_color = layer_get(ctx)->text_color;
  axes[3].title_font = layer_get_font(ctx);
  axes[3].title_font_size = layer_get_font_size(ctx);
  axes[3].title_color = layer_get(ctx)->text_color;
  axes[3].border_style.line_width = unit_from_pt(1, layer_get_dpi(ctx));
  axes[3].border_style.color = layer_get(ctx)->foreground_color;

  auto config_rc = expr_walk_map(expr, {
    {"position", std::bind(&axis_configure_position, _1, &axes)},

    /* scale options */
    {
      "scale-x",
      expr_calln_fn({
        std::bind(&scale_configure_kind, _1, &axes[0].scale),
        std::bind(&scale_configure_kind, _1, &axes[2].scale),
      })
    },
    {
      "scale-y",
      expr_calln_fn({
        std::bind(&scale_configure_kind, _1, &axes[1].scale),
        std::bind(&scale_configure_kind, _1, &axes[3].scale),
      })
    },
    {"scale-top", std::bind(&scale_configure_kind, _1, &axes[0].scale)},
    {"scale-right", std::bind(&scale_configure_kind, _1, &axes[1].scale)},
    {"scale-bottom", std::bind(&scale_configure_kind, _1, &axes[2].scale)},
    {"scale-left", std::bind(&scale_configure_kind, _1, &axes[3].scale)},

    {
      "limit-x",
      expr_calln_fn({
        std::bind(&expr_to_float64_opt_pair, _1, &axes[0].scale.min, &axes[0].scale.max),
        std::bind(&expr_to_float64_opt_pair, _1, &axes[2].scale.min, &axes[2].scale.max),
      })
    },
    {
      "limit-y",
      expr_calln_fn({
        std::bind(&expr_to_float64_opt_pair, _1, &axes[1].scale.min, &axes[1].scale.max),
        std::bind(&expr_to_float64_opt_pair, _1, &axes[3].scale.min, &axes[3].scale.max),
      })
    },
    {"limit-top", std::bind(&expr_to_float64_opt_pair, _1, &axes[0].scale.min, &axes[0].scale.max)},
    {"limit-right", std::bind(&expr_to_float64_opt_pair, _1, &axes[1].scale.min, &axes[1].scale.max)},
    {"limit-bottom", std::bind(&expr_to_float64_opt_pair, _1, &axes[2].scale.min, &axes[2].scale.max)},
    {"limit-left", std::bind(&expr_to_float64_opt_pair, _1, &axes[3].scale.min, &axes[3].scale.max)},

    /* label options */
    {
      "label-placement-x",
      expr_calln_fn({
        std::bind(&scale_configure_layout, _1, &axes[0].label_placement),
        std::bind(&scale_configure_layout, _1, &axes[2].label_placement)
      })
    },
    {
      "label-placement-y",
      expr_calln_fn({
        std::bind(&scale_configure_layout, _1, &axes[1].label_placement),
        std::bind(&scale_configure_layout, _1, &axes[3].label_placement)
      })
    },
    {"label-placement-top", std::bind(&scale_configure_layout, _1, &axes[0].label_placement)},
    {"label-placement-right", std::bind(&scale_configure_layout, _1, &axes[1].label_placement)},
    {"label-placement-bottom", std::bind(&scale_configure_layout, _1, &axes[2].label_placement)},
    {"label-placement-left", std::bind(&scale_configure_layout, _1, &axes[3].label_placement)},
    {
      "label-format",
      expr_calln_fn({
        std::bind(&format_configure, _1, &axes[0].label_formatter),
        std::bind(&format_configure, _1, &axes[1].label_formatter),
        std::bind(&format_configure, _1, &axes[2].label_formatter),
        std::bind(&format_configure, _1, &axes[3].label_formatter)
      })
    },
    {
      "label-format-x",
      expr_calln_fn({
        std::bind(&format_configure, _1, &axes[0].label_formatter),
        std::bind(&format_configure, _1, &axes[2].label_formatter)
      })
    },
    {
      "label-format-y",
      expr_calln_fn({
        std::bind(&format_configure, _1, &axes[1].label_formatter),
        std::bind(&format_configure, _1, &axes[3].label_formatter)
      })
    },
    {
      "label-attach-top",
      expr_to_enum_fn<AxisLabelAttach>(&axes[0].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {
      "label-attach-bottom",
      expr_to_enum_fn<AxisLabelAttach>(&axes[2].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {
      "label-attach-right",
      expr_to_enum_fn<AxisLabelAttach>(&axes[1].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {
      "label-attach-left",
      expr_to_enum_fn<AxisLabelAttach>(&axes[3].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {"label-offset-top", std::bind(&expr_to_float64, _1, &axes[0].label_offset)},
    {"label-offset-right", std::bind(&expr_to_float64, _1, &axes[1].label_offset)},
    {"label-offset-bottom", std::bind(&expr_to_float64, _1, &axes[2].label_offset)},
    {"label-offset-left", std::bind(&expr_to_float64, _1, &axes[3].label_offset)},
    {"label-padding-top", std::bind(&measure_read, _1, &axes[0].label_padding)},
    {"label-padding-right", std::bind(&measure_read, _1, &axes[1].label_padding)},
    {"label-padding-bottom", std::bind(&measure_read, _1, &axes[2].label_padding)},
    {"label-padding-left", std::bind(&measure_read, _1, &axes[3].label_padding)},
    {"label-rotate-top", std::bind(&expr_to_float64, _1, &axes[0].label_rotate)},
    {"label-rotate-right", std::bind(&expr_to_float64, _1, &axes[1].label_rotate)},
    {"label-rotate-bottom", std::bind(&expr_to_float64, _1, &axes[2].label_rotate)},
    {"label-rotate-left", std::bind(&expr_to_float64, _1, &axes[3].label_rotate)},
    {"label-format-top", std::bind(&format_configure, _1, &axes[0].label_formatter)},
    {"label-format-right", std::bind(&format_configure, _1, &axes[1].label_formatter)},
    {"label-format-bottom", std::bind(&format_configure, _1, &axes[2].label_formatter)},
    {"label-format-left", std::bind(&format_configure, _1, &axes[3].label_formatter)},
    {
      "label-font-size",
      expr_calln_fn({
        std::bind(&expr_to_font_size, _1, layer, &axes[0].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[1].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[2].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[3].label_font_size)
      })
    },
    {
      "label-color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &axes[0].label_color),
        std::bind(&color_read, ctx, _1, &axes[1].label_color),
        std::bind(&color_read, ctx, _1, &axes[2].label_color),
        std::bind(&color_read, ctx, _1, &axes[3].label_color)
      })
    },
    {
      "label-color-x",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &axes[0].label_color),
        std::bind(&color_read, ctx, _1, &axes[2].label_color),
      })
    },
    {
      "label-color-y",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &axes[1].label_color),
        std::bind(&color_read, ctx, _1, &axes[3].label_color)
      })
    },
    {"label-color-top", std::bind(&color_read, ctx, _1, &axes[0].label_color)},
    {"label-color-right", std::bind(&color_read, ctx, _1, &axes[1].label_color)},
    {"label-color-bottom", std::bind(&color_read, ctx, _1, &axes[2].label_color)},
    {"label-color-left", std::bind(&color_read, ctx, _1, &axes[3].label_color)},
    {
      "label-font",
      expr_calln_fn({
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[0].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[1].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[2].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[3].label_font))
      })
    },
    {"label-font-top", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[0].label_font))},
    {"label-font-right", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[1].label_font))},
    {"label-font-bottom", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[2].label_font))},
    {"label-font-left", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[3].label_font))},
    {
      "label-font-size-x",
      expr_calln_fn({
        std::bind(&expr_to_font_size, _1, layer, &axes[0].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[2].label_font_size)
      })
    },
    {
      "label-font-size-y",
      expr_calln_fn({
        std::bind(&expr_to_font_size, _1, layer, &axes[1].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[3].label_font_size)
      })
    },
    {"label-font-size-top", std::bind(&expr_to_font_size, _1, layer, &axes[0].label_font_size)},
    {"label-font-size-right", std::bind(&expr_to_font_size, _1, layer, &axes[1].label_font_size)},
    {"label-font-size-bottom", std::bind(&expr_to_font_size, _1, layer, &axes[2].label_font_size)},
    {"label-font-size-left", std::bind(&expr_to_font_size, _1, layer, &axes[3].label_font_size)},

    /* tick options */
    {
      "label-placement-x",
      expr_calln_fn({
        std::bind(&scale_configure_layout, _1, &axes[0].tick_placement),
        std::bind(&scale_configure_layout, _1, &axes[2].tick_placement)
      })
    },
    {
      "label-placement-y",
      expr_calln_fn({
        std::bind(&scale_configure_layout, _1, &axes[1].tick_placement),
        std::bind(&scale_configure_layout, _1, &axes[3].tick_placement)
      })
    },
    {"tick-placement-top", std::bind(&scale_configure_layout, _1, &axes[0].tick_placement)},
    {"tick-placement-right", std::bind(&scale_configure_layout, _1, &axes[1].tick_placement)},
    {"tick-placement-bottom", std::bind(&scale_configure_layout, _1, &axes[2].tick_placement)},
    {"tick-placement-left", std::bind(&scale_configure_layout, _1, &axes[3].tick_placement)},
    {"tick-offset-top", std::bind(&expr_to_float64, _1, &axes[0].tick_offset)},
    {"tick-offset-right", std::bind(&expr_to_float64, _1, &axes[1].tick_offset)},
    {"tick-offset-bottom", std::bind(&expr_to_float64, _1, &axes[2].tick_offset)},
    {"tick-offset-left", std::bind(&expr_to_float64, _1, &axes[3].tick_offset)},
    {"tick-length-top", std::bind(&measure_read, _1, &axes[0].tick_length)},
    {"tick-length-right", std::bind(&measure_read, _1, &axes[1].tick_length)},
    {"tick-length-bottom", std::bind(&measure_read, _1, &axes[2].tick_length)},
    {"tick-length-left", std::bind(&measure_read, _1, &axes[3].tick_length)},

    /* title options */
    {
      "title-x",
      expr_calln_fn({
        std::bind(&expr_to_string, _1, &axes[0].title),
        std::bind(&expr_to_string, _1, &axes[2].title)
      })
    },
    {
      "title-y",
      expr_calln_fn({
        std::bind(&expr_to_string, _1, &axes[1].title),
        std::bind(&expr_to_string, _1, &axes[3].title)
      })
    },
    {"title-top", std::bind(&expr_to_string, _1, &axes[0].title)},
    {"title-right", std::bind(&expr_to_string, _1, &axes[1].title)},
    {"title-bottom", std::bind(&expr_to_string, _1, &axes[2].title)},
    {"title-left", std::bind(&expr_to_string, _1, &axes[3].title)},
    {
      "title-font",
      expr_calln_fn({
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[0].title_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[1].title_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[2].title_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[3].title_font))
      })
    },
    {"title-font-top", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[0].title_font))},
    {"title-font-right", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[1].title_font))},
    {"title-font-bottom", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[2].title_font))},
    {"title-font-left", expr_call_string_fn(std::bind(&font_load_best, _1, &axes[3].title_font))},
    {
      "title-font-size",
      expr_calln_fn({
        std::bind(&expr_to_font_size, _1, layer, &axes[0].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[1].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[2].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[3].title_font_size)
      })
    },
    {"title-font-size-top", std::bind(&expr_to_font_size, _1, layer, &axes[0].title_font_size)},
    {"title-font-size-right", std::bind(&expr_to_font_size, _1, layer, &axes[1].title_font_size)},
    {"title-font-size-bottom", std::bind(&expr_to_font_size, _1, layer, &axes[2].title_font_size)},
    {"title-font-size-left", std::bind(&expr_to_font_size, _1, layer, &axes[3].title_font_size)},
    {
      "title-color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &axes[0].title_color),
        std::bind(&color_read, ctx, _1, &axes[1].title_color),
        std::bind(&color_read, ctx, _1, &axes[2].title_color),
        std::bind(&color_read, ctx, _1, &axes[3].title_color)
      })
    },
    {"title-color-top", std::bind(&color_read, ctx, _1, &axes[0].title_color)},
    {"title-color-right", std::bind(&color_read, ctx, _1, &axes[1].title_color)},
    {"title-color-bottom", std::bind(&color_read, ctx, _1, &axes[2].title_color)},
    {"title-color-left", std::bind(&color_read, ctx, _1, &axes[3].title_color)},
    {"title-offset-top", std::bind(&expr_to_float64, _1, &axes[0].title_offset)},
    {"title-offset-right", std::bind(&expr_to_float64, _1, &axes[1].title_offset)},
    {"title-offset-bottom", std::bind(&expr_to_float64, _1, &axes[2].title_offset)},
    {"title-offset-left", std::bind(&expr_to_float64, _1, &axes[3].title_offset)},
    {"title-padding-top", std::bind(&measure_read, _1, &axes[0].title_padding)},
    {"title-padding-right", std::bind(&measure_read, _1, &axes[1].title_padding)},
    {"title-padding-bottom", std::bind(&measure_read, _1, &axes[2].title_padding)},
    {"title-padding-left", std::bind(&measure_read, _1, &axes[3].title_padding)},
    {"title-rotate-top", std::bind(&expr_to_float64, _1, &axes[0].title_rotate)},
    {"title-rotate-right", std::bind(&expr_to_float64, _1, &axes[1].title_rotate)},
    {"title-rotate-bottom", std::bind(&expr_to_float64, _1, &axes[2].title_rotate)},
    {"title-rotate-left", std::bind(&expr_to_float64, _1, &axes[3].title_rotate)},

    /* margin options */
    {
      "margin",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &margins[0]),
        std::bind(&expr_to_size, _1, layer, &margins[1]),
        std::bind(&expr_to_size, _1, layer, &margins[2]),
        std::bind(&expr_to_size, _1, layer, &margins[3]),
      })
    },
    {"margin-top", std::bind(&expr_to_size, _1, layer, &margins[0])},
    {"margin-right", std::bind(&expr_to_size, _1, layer, &margins[1])},
    {"margin-bottom", std::bind(&expr_to_size, _1, layer, &margins[2])},
    {"margin-left", std::bind(&expr_to_size, _1, layer, &margins[3])},

    /* border options */
    {
      "border-width",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &axes[0].border_style.line_width),
        std::bind(&expr_to_size, _1, layer, &axes[1].border_style.line_width),
        std::bind(&expr_to_size, _1, layer, &axes[2].border_style.line_width),
        std::bind(&expr_to_size, _1, layer, &axes[3].border_style.line_width)
      })
    },
    {"border-width-top", std::bind(&expr_to_size, _1, layer, &axes[0].border_style.line_width)},
    {"border-width-right", std::bind(&expr_to_size, _1, layer, &axes[1].border_style.line_width)},
    {"border-width-bottom", std::bind(&expr_to_size, _1, layer, &axes[2].border_style.line_width)},
    {"border-width-left", std::bind(&expr_to_size, _1, layer, &axes[3].border_style.line_width)},

    {
      "border-color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &axes[0].border_style.color),
        std::bind(&color_read, ctx, _1, &axes[1].border_style.color),
        std::bind(&color_read, ctx, _1, &axes[2].border_style.color),
        std::bind(&color_read, ctx, _1, &axes[3].border_style.color)
      })
    },
    {"border-color-top", std::bind(&color_read, ctx, _1, &axes[0].border_style.color)},
    {"border-color-right", std::bind(&color_read, ctx, _1, &axes[1].border_style.color)},
    {"border-color-bottom", std::bind(&color_read, ctx, _1, &axes[2].border_style.color)},
    {"border-color-left", std::bind(&color_read, ctx, _1, &axes[3].border_style.color)},

    {
      "border-style",
      expr_calln_fn({
        std::bind(&expr_to_stroke_style, _1, &axes[0].border_style),
        std::bind(&expr_to_stroke_style, _1, &axes[1].border_style),
        std::bind(&expr_to_stroke_style, _1, &axes[2].border_style),
        std::bind(&expr_to_stroke_style, _1, &axes[3].border_style)
      })
    },
    {"border-style-top", std::bind(&expr_to_stroke_style, _1, &axes[0].border_style)},
    {"border-style-right", std::bind(&expr_to_stroke_style, _1, &axes[1].border_style)},
    {"border-style-bottom", std::bind(&expr_to_stroke_style, _1, &axes[2].border_style)},
    {"border-style-left", std::bind(&expr_to_stroke_style, _1, &axes[3].border_style)},

    /* global font options */
    {
      "font",
      expr_calln_fn({
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[0].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[1].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[2].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[3].label_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[0].title_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[1].title_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[2].title_font)),
        expr_call_string_fn(std::bind(&font_load_best, _1, &axes[3].title_font))
      })
    },
    {
      "font-size",
      expr_calln_fn({
        std::bind(&expr_to_font_size, _1, layer, &axes[0].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[1].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[2].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[3].title_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[0].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[1].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[2].label_font_size),
        std::bind(&expr_to_font_size, _1, layer, &axes[3].label_font_size),
      })
    }
  });

  if (!config_rc) {
    return config_rc;
  }

  std::array<double, 4> padding = {0, 0, 0, 0};
  for (size_t i = 0; i < 4; ++i) {
    if (!axes[i].enable) {
      continue;
    }

    if (auto rc = axis_set_defaults(ctx, &axes[i]); !rc) {
      return rc;
    }

    axis_layout(axes[i], ctx, &padding[i]);
  }

  auto bbox = layout_margin_box(
      plot_get_clip(plot, layer_get(ctx)),
      margins[0].value + padding[0],
      margins[1].value + padding[1],
      margins[2].value + padding[2],
      margins[3].value + padding[3]);

  plot->layout_stack.push_back(bbox);

  for (auto& a : axes) {
    if (!a.enable) {
      continue;
    }

    if (auto rc = plot_axis(ctx, plot, &a); !rc) {
      return rc;
    }
  }

  return OK;
}

} // namespace clip::plotgen

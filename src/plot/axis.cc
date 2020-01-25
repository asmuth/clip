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

namespace clip::elements::plot::axis {

static const double kDefaultTitlePaddingHorizEM = .6;
static const double kDefaultTitlePaddingVertEM = 1;
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
  Measure title_font_size;
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
  Measure label_font_size;
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

void axis_convert_units(AxisDefinition* config, const Context* ctx) {
  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config->label_font_size);

  convert_unit_typographic(
      ctx->dpi,
      config->label_font_size,
      &config->label_padding);

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config->title_font_size);

  convert_unit_typographic(
      ctx->dpi,
      config->title_font_size,
      &config->title_padding);

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config->border_style.line_width);

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config->tick_length);
}

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
          ctx->dpi,
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
        ctx->dpi,
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
    const AxisDefinition& axis_config,
    const Context* ctx,
    double* margin) {
  auto axis = axis_config;
  axis_convert_units(&axis, ctx);

  /* add margin for the labels */
  {
    *margin += measure_or(
        axis.label_padding,
        from_em(kDefaultLabelPaddingEM, axis.label_font_size));

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
                from_em(kDefaultTitlePaddingVertEM, axis.title_font_size));
        break;
      case AxisAlign::Y:
      case AxisAlign::LEFT:
      case AxisAlign::RIGHT:
        *margin += measure_or(
                axis.title_padding,
                from_em(kDefaultTitlePaddingHorizEM, axis.title_font_size));
        break;
    }

    if (auto rc = axis_layout_title(axis, ctx, margin); !rc) {
      return rc;
    }
  }

  return OK;
}

static ReturnCode axis_draw_vertical(
    Context* ctx,
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
      from_pt(kDefaultTickLengthPT, ctx->dpi));

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
      from_em(kDefaultLabelPaddingEM, axis_config.label_font_size));

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
        from_em(kDefaultTitlePaddingHorizEM, axis_config.title_font_size));

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

static ReturnCode axis_draw_horizontal(
    Context* ctx,
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
      from_pt(kDefaultTickLengthPT, ctx->dpi));

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
      from_em(kDefaultLabelPaddingEM, axis_config.label_font_size));

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
        from_em(kDefaultTitlePaddingVertEM, axis_config.title_font_size));

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

ReturnCode axis_prepare(
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

  axis_convert_units(config, ctx);
  return OK;
}

ReturnCode axis_draw(
    Context* ctx,
    AxisDefinition* config) {
  const auto& axis = *config;
  const auto& bbox = context_get_clip(ctx);

  ReturnCode rc;
  switch (axis.align) {
    case AxisAlign::X:
      rc = axis_draw_horizontal(
          ctx,
          axis,
          bbox.y + bbox.h * 0.5,
          bbox.x,
          bbox.x + bbox.w);
      break;
    case AxisAlign::TOP:
      rc = axis_draw_horizontal(
          ctx,
          axis,
          bbox.y + bbox.h,
          bbox.x,
          bbox.x + bbox.w);
      break;
    case AxisAlign::BOTTOM:
      rc = axis_draw_horizontal(
          ctx,
          axis,
          bbox.y,
          bbox.x,
          bbox.x + bbox.w);
      break;
    case AxisAlign::Y:
      rc = axis_draw_vertical(
          ctx,
          axis,
          bbox.x + bbox.w * 0.5,
          bbox.y,
          bbox.y + bbox.h);
      break;
    case AxisAlign::LEFT:
      rc = axis_draw_vertical(
          ctx,
          axis,
          bbox.x,
          bbox.y,
          bbox.y + bbox.h);
      break;
    case AxisAlign::RIGHT:
      rc = axis_draw_vertical(
          ctx,
          axis,
          bbox.x + bbox.w,
          bbox.y,
          bbox.y + bbox.h);
      break;
  }

  return rc;
}

ReturnCode axis_draw(Context* ctx, const Expr* expr) {
  auto elem_name = expr_get_value(expr);

  auto config = std::make_shared<AxisDefinition>();
  config->label_font = ctx->font;
  config->label_font_size = ctx->font_size;
  config->label_color = ctx->text_color;
  config->title_font = ctx->font;
  config->title_font_size = ctx->font_size;
  config->title_color = ctx->text_color;
  config->border_style.line_width = from_pt(1);
  config->border_style.color = ctx->foreground_color;

  {
    auto rc = expr_walk_map_with_defaults(expr_next(expr), ctx->defaults, {
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
      config->title_offset = 1;
      config->tick_offset = 0;
      config->label_offset = 1;
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
      config->title_offset = -1;
      config->title_rotate = -90;
      config->tick_offset = 0;
      config->label_offset = -1;
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
    auto rc = expr_walk_map_with_defaults(expr_next(expr), ctx->defaults, {
      /* label options */
      {"label-font", expr_call_string_fn(bind(&font_load_best, _1, &config->label_font))},
      {"label-placement", bind(&scale_configure_layout, _1, &config->label_placement)},
      {"label-format", bind(&format_configure, _1, &config->label_formatter)},
      {
        "label-attach",
        expr_to_enum_fn<AxisLabelAttach>(&config->label_attach, {
          { "top", AxisLabelAttach::TOP },
          { "right", AxisLabelAttach::RIGHT },
          { "bottom", AxisLabelAttach::BOTTOM },
          { "left", AxisLabelAttach::LEFT },
        })
      },
      {"label-offset", bind(&expr_to_float64, _1, &config->label_offset)},
      {"label-padding", bind(&measure_read, _1, &config->label_padding)},
      {"label-rotate", bind(&expr_to_float64, _1, &config->label_rotate)},
      {"label-font-size", bind(&measure_read, _1, &config->label_font_size)},
      {"label-color", bind(&color_read, ctx, _1, &config->label_color)},

      /* tick options */
      {"tick-placement", bind(&scale_configure_layout, _1, &config->tick_placement)},
      {"tick-offset", bind(&expr_to_float64, _1, &config->tick_offset)},
      {"tick-length", bind(&measure_read, _1, &config->tick_length)},

      /* scale options */
      {"scale", bind(&scale_configure_kind, _1, &config->scale)},
      {"scale-padding", bind(&expr_to_float64, _1, &config->scale.padding)},
      {"limit", bind(&expr_to_float64_opt_pair, _1, &config->scale.min, &config->scale.max)},
      {"limit-min", bind(&expr_to_float64_opt, _1, &config->scale.min)},
      {"limit-max", bind(&expr_to_float64_opt, _1, &config->scale.max)},

      /* title options */
      {"title", bind(&expr_to_string, _1, &config->title)},
      {"label-font", expr_call_string_fn(bind(&font_load_best, _1, &config->title_font))},
      {"title-font-size", bind(&measure_read, _1, &config->title_font_size)},
      {"title-color", bind(&color_read, ctx, _1, &config->title_color)},
      {"title-offset", bind(&expr_to_float64, _1, &config->title_offset)},
      {"title-padding", bind(&measure_read, _1, &config->title_padding)},
      {"title-rotate", bind(&expr_to_float64, _1, &config->title_rotate)},

      /* border options */
      {"border", bind(&expr_to_stroke_style, _1, &config->border_style)},
      {"border-color", bind(&color_read, ctx, _1, &config->border_style.color)},
      {"border-width", bind(&measure_read, _1, &config->border_style.line_width)},

      /* font options */
      {
        "font",
        expr_calln_fn({
          expr_call_string_fn(bind(&font_load_best, _1, &config->label_font)),
          expr_call_string_fn(bind(&font_load_best, _1, &config->title_font)),
        })
      }
    }, false);

    if (!rc) {
      return rc;
    }
  }

  if (auto rc = axis_prepare(ctx, config.get()); !rc) {
    return rc;
  }

  return axis_draw(ctx, config.get());
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

ReturnCode axis_add_all(Context* ctx, const Expr* expr) {
  std::array<AxisDefinition, 4> axes;

  axes[0].align = AxisAlign::TOP;
  axes[0].label_attach = AxisLabelAttach::BOTTOM;
  axes[0].title_offset = 1;
  axes[0].tick_offset = -1;
  axes[0].label_offset = 1;
  axes[0].label_font = ctx->font;
  axes[0].label_font_size = ctx->font_size;
  axes[0].label_color = ctx->text_color;
  axes[0].title_font = ctx->font;
  axes[0].title_font_size = ctx->font_size;
  axes[0].title_color = ctx->text_color;
  axes[0].border_style.line_width = from_pt(1);
  axes[0].border_style.color = ctx->foreground_color;

  axes[1].align = AxisAlign::RIGHT;
  axes[1].label_attach = AxisLabelAttach::LEFT;
  axes[1].title_offset = 1;
  axes[1].title_rotate = -90;
  axes[1].tick_offset = -1;
  axes[1].label_offset = 1;
  axes[1].label_font = ctx->font;
  axes[1].label_font_size = ctx->font_size;
  axes[1].label_color = ctx->text_color;
  axes[1].title_font = ctx->font;
  axes[1].title_font_size = ctx->font_size;
  axes[1].title_color = ctx->text_color;
  axes[1].border_style.line_width = from_pt(1);
  axes[1].border_style.color = ctx->foreground_color;

  axes[2].align = AxisAlign::BOTTOM;
  axes[2].label_attach = AxisLabelAttach::TOP;
  axes[2].title_offset = -1;
  axes[2].tick_offset = 1;
  axes[2].label_offset = -1;
  axes[2].label_font = ctx->font;
  axes[2].label_font_size = ctx->font_size;
  axes[2].label_color = ctx->text_color;
  axes[2].title_font = ctx->font;
  axes[2].title_font_size = ctx->font_size;
  axes[2].title_color = ctx->text_color;
  axes[2].border_style.line_width = from_pt(1);
  axes[2].border_style.color = ctx->foreground_color;

  axes[3].align = AxisAlign::LEFT;
  axes[3].label_attach = AxisLabelAttach::RIGHT;
  axes[3].title_offset = -1;
  axes[3].title_rotate = 90;
  axes[3].tick_offset = 1;
  axes[3].label_offset = -1;
  axes[3].label_font = ctx->font;
  axes[3].label_font_size = ctx->font_size;
  axes[3].label_color = ctx->text_color;
  axes[3].title_font = ctx->font;
  axes[3].title_font_size = ctx->font_size;
  axes[3].title_color = ctx->text_color;
  axes[3].border_style.line_width = from_pt(1);
  axes[3].border_style.color = ctx->foreground_color;

  auto config_rc = expr_walk_map_with_defaults(expr_next(expr), ctx->defaults, {
    {"position", bind(&axis_configure_position, _1, &axes)},
    {
      "font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].title_font))
      })
    },
    {
      "axis-top-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].label_font)),
      }),
    },
    {
      "axis-right-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].label_font)),
      }),
    },
    {
      "axis-bottom-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].label_font)),
      }),
    },
    {
      "axis-left-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].label_font)),
      }),
    },
    {
      "label-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].label_font))
      })
    },
    {"axis-top-label-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[0].label_font))},
    {"axis-right-label-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[1].label_font))},
    {"axis-bottom-label-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[2].label_font))},
    {"axis-left-label-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[3].label_font))},
    {
      "title-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].title_font))
      })
    },
    {"axis-top-title-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[0].title_font))},
    {"axis-right-title-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[1].title_font))},
    {"axis-bottom-title-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[2].title_font))},
    {"axis-left-title-font", expr_call_string_fn(bind(&font_load_best, _1, &axes[3].title_font))},

    /* label options */
    {
      "label-format",
      expr_calln_fn({
        bind(&format_configure, _1, &axes[0].label_formatter),
        bind(&format_configure, _1, &axes[1].label_formatter),
        bind(&format_configure, _1, &axes[2].label_formatter),
        bind(&format_configure, _1, &axes[3].label_formatter)
      })
    },
    {
      "axis-x-label-format",
      expr_calln_fn({
        bind(&format_configure, _1, &axes[0].label_formatter),
        bind(&format_configure, _1, &axes[2].label_formatter),
      })
    },
    {
      "axis-y-label-format",
      expr_calln_fn({
        bind(&format_configure, _1, &axes[1].label_formatter),
        bind(&format_configure, _1, &axes[3].label_formatter),
      })
    },
    {"axis-top-label-format", bind(&format_configure, _1, &axes[0].label_formatter)},
    {"axis-right-label-format", bind(&format_configure, _1, &axes[1].label_formatter)},
    {"axis-bottom-label-format", bind(&format_configure, _1, &axes[2].label_formatter)},
    {"axis-left-label-format", bind(&format_configure, _1, &axes[3].label_formatter)},

    {
      "font-size",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[0].title_font_size),
        bind(&measure_read, _1, &axes[1].title_font_size),
        bind(&measure_read, _1, &axes[2].title_font_size),
        bind(&measure_read, _1, &axes[3].title_font_size),
        bind(&measure_read, _1, &axes[0].label_font_size),
        bind(&measure_read, _1, &axes[1].label_font_size),
        bind(&measure_read, _1, &axes[2].label_font_size),
        bind(&measure_read, _1, &axes[3].label_font_size),
      })
    },
    {
      "label-font-size",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[0].label_font_size),
        bind(&measure_read, _1, &axes[1].label_font_size),
        bind(&measure_read, _1, &axes[2].label_font_size),
        bind(&measure_read, _1, &axes[3].label_font_size)
      })
    },
    {"axis-top-label-font-size", bind(&measure_read, _1, &axes[0].label_font_size)},
    {"axis-right-label-font-size", bind(&measure_read, _1, &axes[1].label_font_size)},
    {"axis-bottom-label-font-size", bind(&measure_read, _1, &axes[2].label_font_size)},
    {"axis-left-label-font-size", bind(&measure_read, _1, &axes[3].label_font_size)},

    {
      "label-color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &axes[0].label_color),
        bind(&color_read, ctx, _1, &axes[1].label_color),
        bind(&color_read, ctx, _1, &axes[2].label_color),
        bind(&color_read, ctx, _1, &axes[3].label_color)
      })
    },
    {"axis-top-label-color", bind(&color_read, ctx, _1, &axes[0].label_color)},
    {"axis-right-label-color", bind(&color_read, ctx, _1, &axes[1].label_color)},
    {"axis-bottom-label-color", bind(&color_read, ctx, _1, &axes[2].label_color)},
    {"axis-left-label-color", bind(&color_read, ctx, _1, &axes[3].label_color)},

    {
      "axis-x-label-placement",
      expr_calln_fn({
        bind(&scale_configure_layout, _1, &axes[0].label_placement),
        bind(&scale_configure_layout, _1, &axes[2].label_placement)
      })
    },
    {
      "axis-y-label-placement",
      expr_calln_fn({
        bind(&scale_configure_layout, _1, &axes[1].label_placement),
        bind(&scale_configure_layout, _1, &axes[3].label_placement)
      })
    },
    {"axis-top-label-placement", bind(&scale_configure_layout, _1, &axes[0].label_placement)},
    {"axis-right-label-placement", bind(&scale_configure_layout, _1, &axes[1].label_placement)},
    {"axis-bottom-label-placement", bind(&scale_configure_layout, _1, &axes[2].label_placement)},
    {"axis-left-label-placement", bind(&scale_configure_layout, _1, &axes[3].label_placement)},

    {
      "axis-top-label-attach",
      expr_to_enum_fn<AxisLabelAttach>(&axes[0].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {
      "axis-bottom-label-attach",
      expr_to_enum_fn<AxisLabelAttach>(&axes[2].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {
      "axis-right-label-attach",
      expr_to_enum_fn<AxisLabelAttach>(&axes[1].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },
    {
      "axis-left-label-attach",
      expr_to_enum_fn<AxisLabelAttach>(&axes[3].label_attach, {
        { "top", AxisLabelAttach::TOP },
        { "right", AxisLabelAttach::RIGHT },
        { "bottom", AxisLabelAttach::BOTTOM },
        { "left", AxisLabelAttach::LEFT },
      })
    },

    {"axis-top-label-offset", bind(&expr_to_float64, _1, &axes[0].label_offset)},
    {"axis-right-label-offset", bind(&expr_to_float64, _1, &axes[1].label_offset)},
    {"axis-bottom-label-offset", bind(&expr_to_float64, _1, &axes[2].label_offset)},
    {"axis-left-label-offset", bind(&expr_to_float64, _1, &axes[3].label_offset)},

    {"axis-top-label-padding", bind(&measure_read, _1, &axes[0].label_padding)},
    {"axis-right-label-padding", bind(&measure_read, _1, &axes[1].label_padding)},
    {"axis-bottom-label-padding", bind(&measure_read, _1, &axes[2].label_padding)},
    {"axis-left-label-padding", bind(&measure_read, _1, &axes[3].label_padding)},

    {"axis-top-label-rotate", bind(&expr_to_float64, _1, &axes[0].label_rotate)},
    {"axis-right-label-rotate", bind(&expr_to_float64, _1, &axes[1].label_rotate)},
    {"axis-bottom-label-rotate", bind(&expr_to_float64, _1, &axes[2].label_rotate)},
    {"axis-left-label-rotate", bind(&expr_to_float64, _1, &axes[3].label_rotate)},

    /* tick options */
    {"axis-top-tick-placement", bind(&scale_configure_layout, _1, &axes[0].tick_placement)},
    {"axis-right-tick-placement", bind(&scale_configure_layout, _1, &axes[1].tick_placement)},
    {"axis-bottom-tick-placement", bind(&scale_configure_layout, _1, &axes[2].tick_placement)},
    {"axis-left-tick-placement", bind(&scale_configure_layout, _1, &axes[3].tick_placement)},

    {"axis-top-tick-offset", bind(&expr_to_float64, _1, &axes[0].tick_offset)},
    {"axis-right-tick-offset", bind(&expr_to_float64, _1, &axes[1].tick_offset)},
    {"axis-bottom-tick-offset", bind(&expr_to_float64, _1, &axes[2].tick_offset)},
    {"axis-left-tick-offset", bind(&expr_to_float64, _1, &axes[3].tick_offset)},

    {"axis-top-tick-length", bind(&measure_read, _1, &axes[0].tick_length)},
    {"axis-right-tick-length", bind(&measure_read, _1, &axes[1].tick_length)},
    {"axis-bottom-tick-length", bind(&measure_read, _1, &axes[2].tick_length)},
    {"axis-left-tick-length", bind(&measure_read, _1, &axes[3].tick_length)},

    /* scale options */
    {
      "limit-x",
      expr_calln_fn({
        bind(&expr_to_float64_opt_pair, _1, &axes[0].scale.min, &axes[0].scale.max),
        bind(&expr_to_float64_opt_pair, _1, &axes[2].scale.min, &axes[2].scale.max),
      })
    },
    {
      "limit-y",
      expr_calln_fn({
        bind(&expr_to_float64_opt_pair, _1, &axes[1].scale.min, &axes[1].scale.max),
        bind(&expr_to_float64_opt_pair, _1, &axes[3].scale.min, &axes[3].scale.max),
      })
    },
    {
      "scale-x",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &axes[0].scale),
        bind(&scale_configure_kind, _1, &axes[2].scale),
      })
    },
    {
      "scale-y",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &axes[1].scale),
        bind(&scale_configure_kind, _1, &axes[3].scale),
      })
    },

    {"axis-top-scale", bind(&scale_configure_kind, _1, &axes[0].scale)},
    {"axis-top-scale-padding", bind(&expr_to_float64, _1, &axes[0].scale.padding)},
    {"axis-top-limit", bind(&expr_to_float64_opt_pair, _1, &axes[0].scale.min, &axes[0].scale.max)},
    {"axis-top-limit-min", bind(&expr_to_float64_opt, _1, &axes[0].scale.min)},
    {"axis-top-limit-max", bind(&expr_to_float64_opt, _1, &axes[0].scale.max)},
    {"axis-right-scale", bind(&scale_configure_kind, _1, &axes[1].scale)},
    {"axis-right-scale-padding", bind(&expr_to_float64, _1, &axes[1].scale.padding)},
    {"axis-right-limit", bind(&expr_to_float64_opt_pair, _1, &axes[1].scale.min, &axes[1].scale.max)},
    {"axis-right-limit-min", bind(&expr_to_float64_opt, _1, &axes[1].scale.min)},
    {"axis-right-limit-max", bind(&expr_to_float64_opt, _1, &axes[1].scale.max)},
    {"axis-bottom-scale", bind(&scale_configure_kind, _1, &axes[2].scale)},
    {"axis-bottom-scale-padding", bind(&expr_to_float64, _1, &axes[2].scale.padding)},
    {"axis-bottom-limit", bind(&expr_to_float64_opt_pair, _1, &axes[2].scale.min, &axes[2].scale.max)},
    {"axis-bottom-limit-min", bind(&expr_to_float64_opt, _1, &axes[2].scale.min)},
    {"axis-bottom-limit-max", bind(&expr_to_float64_opt, _1, &axes[2].scale.max)},
    {"axis-left-scale", bind(&scale_configure_kind, _1, &axes[3].scale)},
    {"axis-left-scale-padding", bind(&expr_to_float64, _1, &axes[3].scale.padding)},
    {"axis-left-limit", bind(&expr_to_float64_opt_pair, _1, &axes[3].scale.min, &axes[3].scale.max)},
    {"axis-left-limit-min", bind(&expr_to_float64_opt, _1, &axes[3].scale.min)},
    {"axis-left-limit-max", bind(&expr_to_float64_opt, _1, &axes[3].scale.max)},

    /* title options */
    {"axis-top-title", bind(&expr_to_string, _1, &axes[0].title)},
    {"axis-top-title-font-size", bind(&measure_read, _1, &axes[0].title_font_size)},
    {"axis-top-title-color", bind(&color_read, ctx, _1, &axes[0].title_color)},
    {"axis-top-title-offset", bind(&expr_to_float64, _1, &axes[0].title_offset)},
    {"axis-top-title-padding", bind(&measure_read, _1, &axes[0].title_padding)},
    {"axis-top-title-rotate", bind(&expr_to_float64, _1, &axes[0].title_rotate)},

    {"axis-right-title", bind(&expr_to_string, _1, &axes[1].title)},
    {"axis-right-title-font-size", bind(&measure_read, _1, &axes[1].title_font_size)},
    {"axis-right-title-color", bind(&color_read, ctx, _1, &axes[1].title_color)},
    {"axis-right-title-offset", bind(&expr_to_float64, _1, &axes[1].title_offset)},
    {"axis-right-title-padding", bind(&measure_read, _1, &axes[1].title_padding)},
    {"axis-right-title-rotate", bind(&expr_to_float64, _1, &axes[1].title_rotate)},

    {"axis-bottom-title", bind(&expr_to_string, _1, &axes[2].title)},
    {"axis-bottom-title-font-size", bind(&measure_read, _1, &axes[2].title_font_size)},
    {"axis-bottom-title-color", bind(&color_read, ctx, _1, &axes[2].title_color)},
    {"axis-bottom-title-offset", bind(&expr_to_float64, _1, &axes[2].title_offset)},
    {"axis-bottom-title-padding", bind(&measure_read, _1, &axes[2].title_padding)},
    {"axis-bottom-title-rotate", bind(&expr_to_float64, _1, &axes[2].title_rotate)},

    {"axis-left-title", bind(&expr_to_string, _1, &axes[3].title)},
    {"axis-left-title-font-size", bind(&measure_read, _1, &axes[3].title_font_size)},
    {"axis-left-title-color", bind(&color_read, ctx, _1, &axes[3].title_color)},
    {"axis-left-title-offset", bind(&expr_to_float64, _1, &axes[3].title_offset)},
    {"axis-left-title-padding", bind(&measure_read, _1, &axes[3].title_padding)},
    {"axis-left-title-rotate", bind(&expr_to_float64, _1, &axes[3].title_rotate)},

    /* border options */
    {"axis-top-border", bind(&expr_to_stroke_style, _1, &axes[0].border_style)},
    {"axis-top-border-color", bind(&color_read, ctx, _1, &axes[0].border_style.color)},
    {"axis-top-border-width", bind(&measure_read, _1, &axes[0].border_style.line_width)},

    {"axis-right-border", bind(&expr_to_stroke_style, _1, &axes[1].border_style)},
    {"axis-right-border-color", bind(&color_read, ctx, _1, &axes[1].border_style.color)},
    {"axis-right-border-width", bind(&measure_read, _1, &axes[1].border_style.line_width)},

    {"axis-bottom-border", bind(&expr_to_stroke_style, _1, &axes[2].border_style)},
    {"axis-bottom-border-color", bind(&color_read, ctx, _1, &axes[2].border_style.color)},
    {"axis-bottom-border-width", bind(&measure_read, _1, &axes[2].border_style.line_width)},

    {"axis-left-border", bind(&expr_to_stroke_style, _1, &axes[3].border_style)},
    {"axis-left-border-color", bind(&color_read, ctx, _1, &axes[3].border_style.color)},
    {"axis-left-border-width", bind(&measure_read, _1, &axes[3].border_style.line_width)},
  });

  if (!config_rc) {
    return config_rc;
  }

  std::array<double, 4> margins = {0, 0, 0, 0};
  for (size_t i = 0; i < 4; ++i) {
    if (!axes[i].enable) {
      continue;
    }

    if (auto rc = axis_prepare(ctx, &axes[i]); !rc) {
      return rc;
    }

    axis_layout(axes[i], ctx, &margins[i]);
  }

  auto bbox = layout_margin_box(
      context_get_clip(ctx),
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  ctx->layout_stack.push_back(bbox);

  for (auto& a : axes) {
    if (!a.enable) {
      continue;
    }

    if (auto rc = axis_draw(ctx, &a); !rc) {
      return rc;
    }
  }

  return OK;
}

} // namespace clip::elements::plot::axis

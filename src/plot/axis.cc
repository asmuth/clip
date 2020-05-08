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
      {"scale", bind(&scale_configure_kind, _1, &config->scale)},
      {"limit", bind(&expr_to_float64_opt_pair, _1, &config->scale.min, &config->scale.max)},
      {"limit-min", bind(&expr_to_float64_opt, _1, &config->scale.min)},
      {"limit-max", bind(&expr_to_float64_opt, _1, &config->scale.max)},

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

      /* title options */
      {"title", bind(&expr_to_string, _1, &config->title)},
      {"title-font", expr_call_string_fn(bind(&font_load_best, _1, &config->title_font))},
      {"title-font-size", bind(&measure_read, _1, &config->title_font_size)},
      {"title-color", bind(&color_read, ctx, _1, &config->title_color)},
      {"title-offset", bind(&expr_to_float64, _1, &config->title_offset)},
      {"title-padding", bind(&measure_read, _1, &config->title_padding)},
      {"title-rotate", bind(&expr_to_float64, _1, &config->title_rotate)},

      /* border options */
      {"border-width", bind(&measure_read, _1, &config->border_style.line_width)},
      {"border-color", bind(&color_read, ctx, _1, &config->border_style.color)},
      {"border-style", bind(&expr_to_stroke_style, _1, &config->border_style)},

      /* global font options */
      {
        "font",
        expr_calln_fn({
          expr_call_string_fn(bind(&font_load_best, _1, &config->label_font)),
          expr_call_string_fn(bind(&font_load_best, _1, &config->title_font)),
        })
      },
      {
        "font-size",
        expr_calln_fn({
          bind(&measure_read, _1, &config->label_font_size),
          bind(&measure_read, _1, &config->title_font_size),
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
  std::array<Measure, 4> margins = {from_em(1), from_em(1), from_em(1), from_em(1)};
  std::array<AxisDefinition, 4> axes;

  axes[0].scale = ctx->scale_x;
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

  axes[1].scale = ctx->scale_y;
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

  axes[2].scale = ctx->scale_x;
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

  axes[3].scale = ctx->scale_y;
  axes[3].align = AxisAlign::LEFT;
  axes[3].label_attach = AxisLabelAttach::RIGHT;
  axes[3].title_offset = -1;
  axes[3].title_rotate = -90;
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

  auto config_rc = expr_walk_map(expr, {
    {"position", bind(&axis_configure_position, _1, &axes)},

    /* scale options */
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
    {"scale-top", bind(&scale_configure_kind, _1, &axes[0].scale)},
    {"scale-right", bind(&scale_configure_kind, _1, &axes[1].scale)},
    {"scale-bottom", bind(&scale_configure_kind, _1, &axes[2].scale)},
    {"scale-left", bind(&scale_configure_kind, _1, &axes[3].scale)},

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
    {"limit-top", bind(&expr_to_float64_opt_pair, _1, &axes[0].scale.min, &axes[0].scale.max)},
    {"limit-right", bind(&expr_to_float64_opt_pair, _1, &axes[1].scale.min, &axes[1].scale.max)},
    {"limit-bottom", bind(&expr_to_float64_opt_pair, _1, &axes[2].scale.min, &axes[2].scale.max)},
    {"limit-left", bind(&expr_to_float64_opt_pair, _1, &axes[3].scale.min, &axes[3].scale.max)},

    /* label options */
    {
      "label-placement-x",
      expr_calln_fn({
        bind(&scale_configure_layout, _1, &axes[0].label_placement),
        bind(&scale_configure_layout, _1, &axes[2].label_placement)
      })
    },
    {
      "label-placement-y",
      expr_calln_fn({
        bind(&scale_configure_layout, _1, &axes[1].label_placement),
        bind(&scale_configure_layout, _1, &axes[3].label_placement)
      })
    },
    {"label-placement-top", bind(&scale_configure_layout, _1, &axes[0].label_placement)},
    {"label-placement-right", bind(&scale_configure_layout, _1, &axes[1].label_placement)},
    {"label-placement-bottom", bind(&scale_configure_layout, _1, &axes[2].label_placement)},
    {"label-placement-left", bind(&scale_configure_layout, _1, &axes[3].label_placement)},
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
      "label-format-x",
      expr_calln_fn({
        bind(&format_configure, _1, &axes[0].label_formatter),
        bind(&format_configure, _1, &axes[2].label_formatter)
      })
    },
    {
      "label-format-y",
      expr_calln_fn({
        bind(&format_configure, _1, &axes[1].label_formatter),
        bind(&format_configure, _1, &axes[3].label_formatter)
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
    {"label-offset-top", bind(&expr_to_float64, _1, &axes[0].label_offset)},
    {"label-offset-right", bind(&expr_to_float64, _1, &axes[1].label_offset)},
    {"label-offset-bottom", bind(&expr_to_float64, _1, &axes[2].label_offset)},
    {"label-offset-left", bind(&expr_to_float64, _1, &axes[3].label_offset)},
    {"label-padding-top", bind(&measure_read, _1, &axes[0].label_padding)},
    {"label-padding-right", bind(&measure_read, _1, &axes[1].label_padding)},
    {"label-padding-bottom", bind(&measure_read, _1, &axes[2].label_padding)},
    {"label-padding-left", bind(&measure_read, _1, &axes[3].label_padding)},
    {"label-rotate-top", bind(&expr_to_float64, _1, &axes[0].label_rotate)},
    {"label-rotate-right", bind(&expr_to_float64, _1, &axes[1].label_rotate)},
    {"label-rotate-bottom", bind(&expr_to_float64, _1, &axes[2].label_rotate)},
    {"label-rotate-left", bind(&expr_to_float64, _1, &axes[3].label_rotate)},
    {"label-format-top", bind(&format_configure, _1, &axes[0].label_formatter)},
    {"label-format-right", bind(&format_configure, _1, &axes[1].label_formatter)},
    {"label-format-bottom", bind(&format_configure, _1, &axes[2].label_formatter)},
    {"label-format-left", bind(&format_configure, _1, &axes[3].label_formatter)},
    {
      "label-font-size",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[0].label_font_size),
        bind(&measure_read, _1, &axes[1].label_font_size),
        bind(&measure_read, _1, &axes[2].label_font_size),
        bind(&measure_read, _1, &axes[3].label_font_size)
      })
    },
    {
      "label-color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &axes[0].label_color),
        bind(&color_read, ctx, _1, &axes[1].label_color),
        bind(&color_read, ctx, _1, &axes[2].label_color),
        bind(&color_read, ctx, _1, &axes[3].label_color)
      })
    },
    {
      "label-color-x",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &axes[0].label_color),
        bind(&color_read, ctx, _1, &axes[2].label_color),
      })
    },
    {
      "label-color-y",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &axes[1].label_color),
        bind(&color_read, ctx, _1, &axes[3].label_color)
      })
    },
    {"label-color-top", bind(&color_read, ctx, _1, &axes[0].label_color)},
    {"label-color-right", bind(&color_read, ctx, _1, &axes[1].label_color)},
    {"label-color-bottom", bind(&color_read, ctx, _1, &axes[2].label_color)},
    {"label-color-left", bind(&color_read, ctx, _1, &axes[3].label_color)},
    {
      "label-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].label_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].label_font))
      })
    },
    {"label-font-top", expr_call_string_fn(bind(&font_load_best, _1, &axes[0].label_font))},
    {"label-font-right", expr_call_string_fn(bind(&font_load_best, _1, &axes[1].label_font))},
    {"label-font-bottom", expr_call_string_fn(bind(&font_load_best, _1, &axes[2].label_font))},
    {"label-font-left", expr_call_string_fn(bind(&font_load_best, _1, &axes[3].label_font))},
    {
      "label-font-size-x",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[0].label_font_size),
        bind(&measure_read, _1, &axes[2].label_font_size)
      })
    },
    {
      "label-font-size-y",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[1].label_font_size),
        bind(&measure_read, _1, &axes[3].label_font_size)
      })
    },
    {"label-font-size-top", bind(&measure_read, _1, &axes[0].label_font_size)},
    {"label-font-size-right", bind(&measure_read, _1, &axes[1].label_font_size)},
    {"label-font-size-bottom", bind(&measure_read, _1, &axes[2].label_font_size)},
    {"label-font-size-left", bind(&measure_read, _1, &axes[3].label_font_size)},

    /* tick options */
    {
      "label-placement-x",
      expr_calln_fn({
        bind(&scale_configure_layout, _1, &axes[0].tick_placement),
        bind(&scale_configure_layout, _1, &axes[2].tick_placement)
      })
    },
    {
      "label-placement-y",
      expr_calln_fn({
        bind(&scale_configure_layout, _1, &axes[1].tick_placement),
        bind(&scale_configure_layout, _1, &axes[3].tick_placement)
      })
    },
    {"tick-placement-top", bind(&scale_configure_layout, _1, &axes[0].tick_placement)},
    {"tick-placement-right", bind(&scale_configure_layout, _1, &axes[1].tick_placement)},
    {"tick-placement-bottom", bind(&scale_configure_layout, _1, &axes[2].tick_placement)},
    {"tick-placement-left", bind(&scale_configure_layout, _1, &axes[3].tick_placement)},
    {"tick-offset-top", bind(&expr_to_float64, _1, &axes[0].tick_offset)},
    {"tick-offset-right", bind(&expr_to_float64, _1, &axes[1].tick_offset)},
    {"tick-offset-bottom", bind(&expr_to_float64, _1, &axes[2].tick_offset)},
    {"tick-offset-left", bind(&expr_to_float64, _1, &axes[3].tick_offset)},
    {"tick-length-top", bind(&measure_read, _1, &axes[0].tick_length)},
    {"tick-length-right", bind(&measure_read, _1, &axes[1].tick_length)},
    {"tick-length-bottom", bind(&measure_read, _1, &axes[2].tick_length)},
    {"tick-length-left", bind(&measure_read, _1, &axes[3].tick_length)},

    /* title options */
    {
      "title-x",
      expr_calln_fn({
        bind(&expr_to_string, _1, &axes[0].title),
        bind(&expr_to_string, _1, &axes[2].title)
      })
    },
    {
      "title-y",
      expr_calln_fn({
        bind(&expr_to_string, _1, &axes[1].title),
        bind(&expr_to_string, _1, &axes[3].title)
      })
    },
    {"title-top", bind(&expr_to_string, _1, &axes[0].title)},
    {"title-right", bind(&expr_to_string, _1, &axes[1].title)},
    {"title-bottom", bind(&expr_to_string, _1, &axes[2].title)},
    {"title-left", bind(&expr_to_string, _1, &axes[3].title)},
    {
      "title-font",
      expr_calln_fn({
        expr_call_string_fn(bind(&font_load_best, _1, &axes[0].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[1].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[2].title_font)),
        expr_call_string_fn(bind(&font_load_best, _1, &axes[3].title_font))
      })
    },
    {"title-font-top", expr_call_string_fn(bind(&font_load_best, _1, &axes[0].title_font))},
    {"title-font-right", expr_call_string_fn(bind(&font_load_best, _1, &axes[1].title_font))},
    {"title-font-bottom", expr_call_string_fn(bind(&font_load_best, _1, &axes[2].title_font))},
    {"title-font-left", expr_call_string_fn(bind(&font_load_best, _1, &axes[3].title_font))},
    {
      "title-font-size",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[0].title_font_size),
        bind(&measure_read, _1, &axes[1].title_font_size),
        bind(&measure_read, _1, &axes[2].title_font_size),
        bind(&measure_read, _1, &axes[3].title_font_size)
      })
    },
    {"title-font-size-top", bind(&measure_read, _1, &axes[0].title_font_size)},
    {"title-font-size-right", bind(&measure_read, _1, &axes[1].title_font_size)},
    {"title-font-size-bottom", bind(&measure_read, _1, &axes[2].title_font_size)},
    {"title-font-size-left", bind(&measure_read, _1, &axes[3].title_font_size)},
    {
      "title-color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &axes[0].title_color),
        bind(&color_read, ctx, _1, &axes[1].title_color),
        bind(&color_read, ctx, _1, &axes[2].title_color),
        bind(&color_read, ctx, _1, &axes[3].title_color)
      })
    },
    {"title-color-top", bind(&color_read, ctx, _1, &axes[0].title_color)},
    {"title-color-right", bind(&color_read, ctx, _1, &axes[1].title_color)},
    {"title-color-bottom", bind(&color_read, ctx, _1, &axes[2].title_color)},
    {"title-color-left", bind(&color_read, ctx, _1, &axes[3].title_color)},
    {"title-offset-top", bind(&expr_to_float64, _1, &axes[0].title_offset)},
    {"title-offset-right", bind(&expr_to_float64, _1, &axes[1].title_offset)},
    {"title-offset-bottom", bind(&expr_to_float64, _1, &axes[2].title_offset)},
    {"title-offset-left", bind(&expr_to_float64, _1, &axes[3].title_offset)},
    {"title-padding-top", bind(&measure_read, _1, &axes[0].title_padding)},
    {"title-padding-right", bind(&measure_read, _1, &axes[1].title_padding)},
    {"title-padding-bottom", bind(&measure_read, _1, &axes[2].title_padding)},
    {"title-padding-left", bind(&measure_read, _1, &axes[3].title_padding)},
    {"title-rotate-top", bind(&expr_to_float64, _1, &axes[0].title_rotate)},
    {"title-rotate-right", bind(&expr_to_float64, _1, &axes[1].title_rotate)},
    {"title-rotate-bottom", bind(&expr_to_float64, _1, &axes[2].title_rotate)},
    {"title-rotate-left", bind(&expr_to_float64, _1, &axes[3].title_rotate)},

    /* margin options */
    {
      "margin",
      expr_calln_fn({
        bind(&measure_read, _1, &margins[0]),
        bind(&measure_read, _1, &margins[1]),
        bind(&measure_read, _1, &margins[2]),
        bind(&measure_read, _1, &margins[3]),
      })
    },
    {"margin-top", bind(&measure_read, _1, &margins[0])},
    {"margin-right", bind(&measure_read, _1, &margins[1])},
    {"margin-bottom", bind(&measure_read, _1, &margins[2])},
    {"margin-left", bind(&measure_read, _1, &margins[3])},

    /* border options */
    {
      "border-width",
      expr_calln_fn({
        bind(&measure_read, _1, &axes[0].border_style.line_width),
        bind(&measure_read, _1, &axes[1].border_style.line_width),
        bind(&measure_read, _1, &axes[2].border_style.line_width),
        bind(&measure_read, _1, &axes[3].border_style.line_width)
      })
    },
    {"border-width-top", bind(&measure_read, _1, &axes[0].border_style.line_width)},
    {"border-width-right", bind(&measure_read, _1, &axes[1].border_style.line_width)},
    {"border-width-bottom", bind(&measure_read, _1, &axes[2].border_style.line_width)},
    {"border-width-left", bind(&measure_read, _1, &axes[3].border_style.line_width)},

    {
      "border-color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &axes[0].border_style.color),
        bind(&color_read, ctx, _1, &axes[1].border_style.color),
        bind(&color_read, ctx, _1, &axes[2].border_style.color),
        bind(&color_read, ctx, _1, &axes[3].border_style.color)
      })
    },
    {"border-color-top", bind(&color_read, ctx, _1, &axes[0].border_style.color)},
    {"border-color-right", bind(&color_read, ctx, _1, &axes[1].border_style.color)},
    {"border-color-bottom", bind(&color_read, ctx, _1, &axes[2].border_style.color)},
    {"border-color-left", bind(&color_read, ctx, _1, &axes[3].border_style.color)},

    {
      "border-style",
      expr_calln_fn({
        bind(&expr_to_stroke_style, _1, &axes[0].border_style),
        bind(&expr_to_stroke_style, _1, &axes[1].border_style),
        bind(&expr_to_stroke_style, _1, &axes[2].border_style),
        bind(&expr_to_stroke_style, _1, &axes[3].border_style)
      })
    },
    {"border-style-top", bind(&expr_to_stroke_style, _1, &axes[0].border_style)},
    {"border-style-right", bind(&expr_to_stroke_style, _1, &axes[1].border_style)},
    {"border-style-bottom", bind(&expr_to_stroke_style, _1, &axes[2].border_style)},
    {"border-style-left", bind(&expr_to_stroke_style, _1, &axes[3].border_style)},

    /* global font options */
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
    }
  });

  if (!config_rc) {
    return config_rc;
  }

  for (auto& m : margins) {
    convert_unit_typographic(ctx->dpi, ctx->font_size, &m);
  }

  std::array<double, 4> padding = {0, 0, 0, 0};
  for (size_t i = 0; i < 4; ++i) {
    if (!axes[i].enable) {
      continue;
    }

    if (auto rc = axis_prepare(ctx, &axes[i]); !rc) {
      return rc;
    }

    axis_layout(axes[i], ctx, &padding[i]);
  }

  auto bbox = layout_margin_box(
      context_get_clip(ctx),
      margins[0] + padding[0],
      margins[1] + padding[1],
      margins[2] + padding[2],
      margins[3] + padding[3]);

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

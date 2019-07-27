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
#include "axis.h"

#include "data.h"
#include "environment.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "graphics/brush.h"
#include "core/scale.h"
#include "core/format.h"
#include "core/layout.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <assert.h>

using namespace std::placeholders;

namespace fviz::elements::plot::axis {

static const double kDefaultTitlePaddingHorizEM = 1.4;
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
    title_offset(0.0),
    title_rotate(0.0),
    tick_offset(0.0),
    label_offset(0.0),
    label_rotate(0.0) {}

void axis_convert_units(AxisDefinition* config, const Layer& layer) {
  convert_unit_typographic(
      layer.dpi,
      layer.font_size,
      &config->label_font_size);

  convert_unit_typographic(
      layer.dpi,
      config->label_font_size,
      &config->label_padding);

  convert_unit_typographic(
      layer.dpi,
      layer.font_size,
      &config->title_font_size);

  convert_unit_typographic(
      layer.dpi,
      config->title_font_size,
      &config->title_padding);

  convert_unit_typographic(
      layer.dpi,
      layer.font_size,
      &config->border_style.line_width);

  convert_unit_typographic(
      layer.dpi,
      layer.font_size,
      &config->tick_length);
}

ReturnCode axis_layout_labels(
    const AxisDefinition& axis,
    const Layer& layer,
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
    if (auto rc = text::text_measure_line(
          label_text,
          TextDirection::LTR,
          axis.label_font,
          axis.label_font_size,
          layer.dpi,
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
    const Layer& layer,
    double* margin) {
  Rectangle title_bbox;
  if (auto rc = text::text_measure_line(
        axis.title,
        TextDirection::LTR,
        axis.title_font,
        axis.title_font_size,
        layer.dpi,
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
    const Layer& layer,
    double* margin) {
  auto axis = axis_config;
  axis_convert_units(&axis, layer);

  /* add margin for the labels */
  {
    *margin += measure_or(
        axis.label_padding,
        from_em(kDefaultLabelPaddingEM, axis.label_font_size));

    if (auto rc = axis_layout_labels(axis, layer, margin); !rc) {
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

    if (auto rc = axis_layout_title(axis, layer, margin); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode axis_calculate_size(
    std::shared_ptr<AxisDefinition> axis,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  double size = 0.0;
  axis_layout(*axis, layer, &size);

  switch (axis->align) {
    case AxisAlign::X:
    case AxisAlign::TOP:
    case AxisAlign::BOTTOM:
      *min_height = size;
      break;
    case AxisAlign::Y:
    case AxisAlign::RIGHT:
    case AxisAlign::LEFT:
      *min_width = size;
      break;
  }

  return OK;
}

static ReturnCode axis_draw_vertical(
    const AxisDefinition& axis_config,
    double x,
    double y0,
    double y1,
    Layer* target) {
  /* draw axis line */
  strokeLine(target, {x, y0}, {x, y1}, axis_config.border_style);

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
      from_pt(kDefaultTickLengthPT, target->dpi));

  for (const auto& tick : ticks.positions) {
    auto ty = y0 + (y1 - y0) * (1.0 - tick);
    auto tx = x - tick_length + tick_length * (tick_position + 1) / 2.0;

    strokeLine(
        target,
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
  if (auto rc =axis_layout_labels(axis_config, *target, &label_size);
      !rc) {
    return rc;
  }

  for (size_t i = 0; i < labels.positions.size(); ++i) {
    auto tick = labels.positions[i];
    auto label_text = labels.labels[i];

    Point p;
    p.x = x + label_padding * label_position;
    p.y = y0 + (y1 - y0) * (1.0 - tick);

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

    if (auto rc = drawTextLabel(label_text, p, ax, ay, a, style, target); rc != OK) {
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
          axis_layout_title(axis_config, *target, &title_size);
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
      drawTextLabel(
          axis_config.title,
          p,
          HAlign::CENTER,
          VAlign::CENTER,
          axis_config.title_rotate,
          style,
          target);

    if (!draw_rc) {
      return draw_rc;
    }
  }

  return OK;
}

static ReturnCode axis_draw_horizontal(
    const AxisDefinition& axis_config,
    double y,
    double x0,
    double x1,
    Layer* target) {
  /* draw axis line */
  strokeLine(target, {x0, y}, {x1, y}, axis_config.border_style);

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
      from_pt(kDefaultTickLengthPT, target->dpi));

  for (const auto& tick : ticks.positions) {
    auto ty = y - tick_length + tick_length * (tick_position + 1) / 2.0;
    auto tx = x0 + (x1 - x0) * tick;
    strokeLine(
        target,
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
  if (auto rc = axis_layout_labels(axis_config, *target, &label_size); !rc) {
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

    if (auto rc = drawTextLabel(label_text, p, ax, ay, a, style, target); !rc) {
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
    if (auto rc = axis_layout_title(axis_config, *target, &title_size); !rc) {
      return rc;
    }

    Point p;
    p.x = x0 + (x1 - x0) * 0.5;
    p.y = y + (title_padding + title_size * 0.5) * title_position;

    TextStyle style;
    style.font = axis_config.title_font;
    style.color = axis_config.title_color;
    style.font_size = axis_config.title_font_size;

    auto draw_rc = drawTextLabel(
        axis_config.title,
        p,
        HAlign::CENTER,
        VAlign::CENTER,
        axis_config.title_rotate,
        style,
        target);

    if (!draw_rc) {
      return draw_rc;
    }
  }

  return OK;
}

ReturnCode axis_draw(
    std::shared_ptr<AxisDefinition> config,
    const LayoutInfo& layout,
    Layer* layer) {
  axis_convert_units(config.get(), *layer);
  const auto& axis = *config;

  ReturnCode rc;
  switch (axis.align) {
    case AxisAlign::X:
      rc = axis_draw_horizontal(
          axis,
          layout.content_box.y + layout.content_box.h * 0.5,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          layer);
      break;
    case AxisAlign::TOP:
      rc = axis_draw_horizontal(
          axis,
          layout.content_box.y,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          layer);
      break;
    case AxisAlign::BOTTOM:
      rc = axis_draw_horizontal(
          axis,
          layout.content_box.y + layout.content_box.h,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          layer);
      break;
    case AxisAlign::Y:
      rc = axis_draw_vertical(
          axis,
          layout.content_box.x + layout.content_box.w * 0.5,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          layer);
      break;
    case AxisAlign::LEFT:
      rc = axis_draw_vertical(
          axis,
          layout.content_box.x,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          layer);
      break;
    case AxisAlign::RIGHT:
      rc = axis_draw_vertical(
          axis,
          layout.content_box.x + layout.content_box.w,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          layer);
      break;
  }

  return rc;
}

ReturnCode build(const Environment& env, const Expr* expr, ElementRef* elem) {
  auto elem_name = expr_get_value(expr);

  auto config = std::make_shared<AxisDefinition>();
  config->label_font = env.font;
  config->label_font_size = env.font_size;
  config->label_color = env.text_color;
  config->title_font = env.font;
  config->title_font_size = env.font_size;
  config->title_color = env.text_color;
  config->border_style.line_width = from_pt(1);
  config->border_style.color = env.foreground_color;

  {
    auto rc = expr_walk_map(expr_next(expr), {
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
      config->label_attach = AxisLabelAttach::TOP;
      config->title_offset = 1;
      config->tick_offset = -1;
      config->label_offset = 1;
      break;
    case AxisAlign::BOTTOM:
      config->label_attach = AxisLabelAttach::BOTTOM;
      config->title_offset = -1;
      config->tick_offset = 1;
      config->label_offset = -1;
      break;
    case AxisAlign::Y:
      config->label_attach = AxisLabelAttach::LEFT;
      config->title_offset = 1;
      config->title_rotate = -90;
      config->tick_offset = 0;
      config->label_offset = 1;
      break;
    case AxisAlign::LEFT:
      config->label_attach = AxisLabelAttach::LEFT;
      config->title_offset = 1;
      config->title_rotate = -90;
      config->tick_offset = -1;
      config->label_offset = 1;
      break;
    case AxisAlign::RIGHT:
      config->label_attach = AxisLabelAttach::RIGHT;
      config->title_offset = -1;
      config->title_rotate = -90;
      config->tick_offset = 1;
      config->label_offset = -1;
      break;
  };

  {
    auto rc = expr_walk_map(expr_next(expr), {
      /* label options */
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
      {"label-padding", bind(&expr_to_measure, _1, &config->label_padding)},
      {"label-rotate", bind(&expr_to_float64, _1, &config->label_rotate)},
      {"label-font-size", bind(&expr_to_measure, _1, &config->label_font_size)},
      {"label-color", bind(&expr_to_color, _1, &config->label_color)},

      /* tick options */
      {"tick-placement", bind(&scale_configure_layout, _1, &config->tick_placement)},
      {"tick-offset", bind(&expr_to_float64, _1, &config->tick_offset)},
      {"tick-length", bind(&expr_to_measure, _1, &config->tick_length)},

      /* scale options */
      {"scale", bind(&scale_configure_kind, _1, &config->scale)},
      {"scale-padding", bind(&expr_to_float64, _1, &config->scale.padding)},
      {"limit", bind(&expr_to_float64_opt_pair, _1, &config->scale.min, &config->scale.max)},
      {"limit-min", bind(&expr_to_float64_opt, _1, &config->scale.min)},
      {"limit-max", bind(&expr_to_float64_opt, _1, &config->scale.max)},

      /* title options */
      {"title", bind(&expr_to_string, _1, &config->title)},
      {"title-font-size", bind(&expr_to_measure, _1, &config->title_font_size)},
      {"title-color", bind(&expr_to_color, _1, &config->title_color)},
      {"title-offset", bind(&expr_to_float64, _1, &config->title_offset)},
      {"title-padding", bind(&expr_to_measure, _1, &config->title_padding)},
      {"title-rotate", bind(&expr_to_float64, _1, &config->title_rotate)},

      /* border options */
      {"border", bind(&expr_to_stroke_style, _1, &config->border_style)},
      {"border-color", bind(&expr_to_color, _1, &config->border_style.color)},
      {"border-width", bind(&expr_to_measure, _1, &config->border_style.line_width)},
    }, false);

    if (!rc) {
      return rc;
    }
  }

  /* setup defaults */
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

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&axis_draw, config, _1, _2);
  (*elem)->size_hint = bind(&axis_calculate_size, config, _1, _2, _3, _4, _5);
  return OK;
}

} // namespace fviz::elements::plot::axis

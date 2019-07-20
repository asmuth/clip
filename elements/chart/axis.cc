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

namespace fviz::elements::chart::axis {

static const double kDefaultTitlePaddingEM = 0.8;
static const double kDefaultLabelPaddingEM = 0.8;
static const double kDefaultLineWidthPT = 1;
static const double kDefaultTickLengthPT = 4;

enum class AxisPosition {
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
  CENTER_HORIZ,
  CENTER_VERT
};

enum class AxisLabelPosition {
  LEFT,
  RIGHT,
  TOP,
  BOTTOM,
  INSIDE,
  OUTSIDE,
};

struct AxisDefinition;

using AxisLabelPlacement = std::function<ReturnCode (
    const ScaleConfig& domain,
    AxisDefinition*)>;

struct AxisDefinition {
  AxisDefinition();
  AxisPosition position;
  ScaleConfig scale;
  ScaleLayoutFn scale_layout;
  std::string title;
  FontInfo title_font;
  Measure title_font_size;
  Color title_color;
  Measure title_padding;
  double title_rotate;
  AxisLabelPosition title_position;
  AxisLabelPosition tick_position;
  AxisLabelPosition label_position;
  StrokeStyle border_style;
  Formatter label_formatter;
  Measure label_padding;
  FontInfo label_font;
  Measure label_font_size;
  Color label_color;
  Measure tick_length;
  LayoutSettings layout;
};

AxisDefinition::AxisDefinition() :
    position(AxisPosition::BOTTOM),
    title_rotate(0.0),
    label_position(AxisLabelPosition::OUTSIDE),
    tick_position(AxisLabelPosition::INSIDE) {}

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

std::string axis_get_label(
    const AxisDefinition& axis,
    size_t idx,
    double offset) {
  const auto& domain = axis.scale;
  auto value = scale_untranslate(domain, offset);
  return axis.label_formatter(idx, std::to_string(value));
}

ReturnCode axis_layout_labels(
    const AxisDefinition& axis,
    const AxisPosition& axis_position,
    const Layer& layer,
    double* margin) {
  /* compute scale layout */
  ScaleLayout slayout;
  axis.scale_layout(axis.scale, &slayout);

  /* compute label margin */
  double label_margin = 0;
  for (size_t i = 0; i < slayout.labels.size(); ++i) {
    auto tick = slayout.labels[i];
    auto label_text = axis_get_label(axis, i, tick);

    TextStyle style;
    style.font = axis.label_font;
    style.font_size = axis.label_font_size;

    Rectangle label_bbox;
    if (auto rc = text::text_measure_span(
          label_text,
          axis.label_font,
          axis.label_font_size,
          layer.dpi,
          layer.text_shaper.get(),
          &label_bbox);
         rc != Status::OK) {
      return rc;
    }

    switch (axis_position) {
      case AxisPosition::TOP:
      case AxisPosition::BOTTOM:
      case AxisPosition::CENTER_HORIZ:
        label_margin = std::max(label_margin, label_bbox.h);
        break;
      case AxisPosition::LEFT:
      case AxisPosition::RIGHT:
      case AxisPosition::CENTER_VERT:
        label_margin = std::max(label_margin, label_bbox.w);
        break;
    }
  }

  *margin += label_margin;
  return OK;
}

ReturnCode axis_layout_title(
    const AxisDefinition& axis,
    const AxisPosition& axis_position,
    const Layer& layer,
    double* margin) {
  Rectangle title_bbox;
  if (auto rc = text::text_measure_span(
        axis.title,
        axis.title_font,
        axis.title_font_size,
        layer.dpi,
        layer.text_shaper.get(),
        &title_bbox);
       rc != Status::OK) {
    return rc;
  }

  switch (axis_position) {
    case AxisPosition::TOP:
    case AxisPosition::BOTTOM:
    case AxisPosition::CENTER_HORIZ:
      *margin += title_bbox.h;
      break;
    case AxisPosition::LEFT:
    case AxisPosition::RIGHT:
    case AxisPosition::CENTER_VERT:
      *margin += title_bbox.w;
      break;
  }

  return OK;
}

ReturnCode axis_layout(
    const AxisDefinition& axis_config,
    const AxisPosition& axis_position,
    const Layer& layer,
    double* margin) {
  auto axis = axis_config;
  axis_convert_units(&axis, layer);

  /* add margin for ticks */
  bool reflow_ticks = false;
  switch (axis.label_position) {
    case AxisLabelPosition::OUTSIDE:
      reflow_ticks = true;
      break;
    case AxisLabelPosition::INSIDE:
      reflow_ticks = false;
      break;
    case AxisLabelPosition::TOP:
      reflow_ticks = (axis_position == AxisPosition::TOP);
      break;
    case AxisLabelPosition::RIGHT:
      reflow_ticks = (axis_position == AxisPosition::RIGHT);
      break;
    case AxisLabelPosition::BOTTOM:
      reflow_ticks = (axis_position == AxisPosition::BOTTOM);
      break;
    case AxisLabelPosition::LEFT:
      reflow_ticks = (axis_position == AxisPosition::LEFT);
      break;
  }

  /* add margin for labels */
  bool reflow_labels = true;
  switch (axis.label_position) {
    case AxisLabelPosition::OUTSIDE:
      reflow_labels = true;
      break;
    case AxisLabelPosition::INSIDE:
      reflow_labels = false;
      break;
    case AxisLabelPosition::TOP:
      reflow_labels = (axis_position == AxisPosition::TOP);
      break;
    case AxisLabelPosition::RIGHT:
      reflow_labels = (axis_position == AxisPosition::RIGHT);
      break;
    case AxisLabelPosition::BOTTOM:
      reflow_labels = (axis_position == AxisPosition::BOTTOM);
      break;
    case AxisLabelPosition::LEFT:
      reflow_labels = (axis_position == AxisPosition::LEFT);
      break;
  }

  if (reflow_labels) {
    *margin += measure_or(
        axis.label_padding,
        from_em(kDefaultLabelPaddingEM, axis.label_font_size));

    if (auto rc = axis_layout_labels(
          axis,
          axis_position,
          layer,
          margin);
          !rc) {
      return rc;
    }
  }

  /* add margin for the title */
  if (!axis.title.empty()) {
    *margin += measure_or(
            axis.title_padding,
            from_em(kDefaultTitlePaddingEM, axis.title_font_size));

    if (auto rc = axis_layout_title(
          axis,
          axis_position,
          layer,
          margin);
          !rc) {
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
  axis_layout(*axis, axis->position, layer, &size);

  switch (axis->position) {
    case AxisPosition::TOP:
    case AxisPosition::BOTTOM:
      *min_height = size;
      break;
    case AxisPosition::RIGHT:
    case AxisPosition::LEFT:
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
  /* compute layout */
  ScaleLayout slayout;
  axis_config.scale_layout(axis_config.scale, &slayout);

  /* draw axis line */
  strokeLine(target, {x, y0}, {x, y1}, axis_config.border_style);

  /* draw ticks */
  double tick_position = 0;
  switch (axis_config.tick_position) {
    case AxisLabelPosition::RIGHT:
      tick_position = 1;
      break;
    case AxisLabelPosition::LEFT:
      tick_position = -1;
      break;
    default:
      break;
  }

  auto tick_length = measure_or(
      axis_config.tick_length,
      from_pt(kDefaultTickLengthPT, target->dpi));

  for (const auto& tick : slayout.ticks) {
    auto y = y0 + (y1 - y0) * (1.0 - tick);
    strokeLine(
        target,
        {x, y},
        {x + tick_length * tick_position, y},
        axis_config.border_style);
  }

  /* draw labels */
  double label_position = 0;
  switch (axis_config.label_position) {
    case AxisLabelPosition::RIGHT:
      label_position = 1;
      break;
    case AxisLabelPosition::LEFT:
      label_position = -1;
      break;
    default:
      break;
  }

  auto label_padding = measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, axis_config.label_font_size));

  double label_size = label_padding;
  if (auto rc =
        axis_layout_labels(
            axis_config,
            axis_config.position,
            *target,
            &label_size);
      !rc) {
    return rc;
  }

  for (size_t i = 0; i < slayout.labels.size(); ++i) {
    auto tick = slayout.labels[i];
    auto label_text = axis_get_label(axis_config, i, tick);

    Point p;
    p.x = x + label_padding * label_position;
    p.y = y0 + (y1 - y0) * (1.0 - tick);

    TextStyle style;
    style.font = axis_config.label_font;
    style.color = axis_config.label_color;
    style.font_size = axis_config.label_font_size;

    auto ax = label_position > 0 ? HAlign::LEFT : HAlign::RIGHT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); rc != OK) {
      return rc;
    }
  }

  /* draw title */
  if (!axis_config.title.empty()) {
    double title_position = 0;
    switch (axis_config.title_position) {
      case AxisLabelPosition::RIGHT:
        title_position = 1;
        break;
      case AxisLabelPosition::LEFT:
        title_position = -1;
        break;
      default:
        break;
    }

    double title_padding = measure_or(
        axis_config.title_padding,
        from_em(kDefaultTitlePaddingEM, axis_config.title_font_size));

    if (title_position == label_position) {
      title_padding += label_size;
    }

    double title_size = 0;
    if (auto rc =
          axis_layout_title(
              axis_config,
              axis_config.position,
              *target,
              &title_size);
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
  /* compute layout */
  ScaleLayout slayout;
  axis_config.scale_layout(axis_config.scale, &slayout);

  /* draw axis line */
  strokeLine(target, {x0, y}, {x1, y}, axis_config.border_style);

  /* draw ticks */
  double tick_position = 0;
  switch (axis_config.tick_position) {
    case AxisLabelPosition::BOTTOM:
      tick_position = 1;
      break;
    case AxisLabelPosition::TOP:
      tick_position = -1;
      break;
    default:
      break;
  }

  auto tick_length = measure_or(
      axis_config.tick_length,
      from_pt(kDefaultTickLengthPT, target->dpi));

  for (const auto& tick : slayout.ticks) {
    auto x = x0 + (x1 - x0) * tick;
    strokeLine(
        target,
        {x, y},
        {x, y + tick_length * tick_position},
        axis_config.border_style);
  }

  /* draw labels */
  double label_position = 0;
  switch (axis_config.label_position) {
    case AxisLabelPosition::BOTTOM:
      label_position = 1;
      break;
    case AxisLabelPosition::TOP:
      label_position = -1;
      break;
    default:
      break;
  }

  double label_padding =  measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, axis_config.label_font_size));

  double label_size = label_padding;
  if (auto rc =
        axis_layout_labels(
            axis_config,
            axis_config.position,
            *target,
            &label_size);
      !rc) {
    return rc;
  }

  for (size_t i = 0; i < slayout.labels.size(); ++i) {
    auto tick = slayout.labels[i];
    auto label_text = axis_get_label(axis_config, i, tick);

    Point p;
    p.x = x0 + (x1 - x0) * tick;
    p.y = y + label_padding * label_position;

    TextStyle style;
    style.font = axis_config.label_font;
    style.color = axis_config.label_color;
    style.font_size = axis_config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = label_position > 0 ? VAlign::TOP : VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); !rc) {
      return rc;
    }
  }

  /* draw title */
  if (!axis_config.title.empty()) {
    double title_position = 0;
    switch (axis_config.title_position) {
      case AxisLabelPosition::BOTTOM:
        title_position = 1;
        break;
      case AxisLabelPosition::TOP:
        title_position = -1;
        break;
      default:
        break;
    }

    double title_padding = measure_or(
        axis_config.title_padding,
        from_em(kDefaultTitlePaddingEM, axis_config.title_font_size));

    if (title_position == label_position) {
      title_padding += label_size;
    }

    double title_size = 0;
    if (auto rc =
          axis_layout_title(
              axis_config,
              axis_config.position,
              *target,
              &title_size);
        !rc) {
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
  switch (axis.position) {
    case AxisPosition::LEFT:
      rc = axis_draw_vertical(
          axis,
          layout.content_box.x + layout.content_box.w,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          layer);
      break;
    case AxisPosition::RIGHT:
      rc = axis_draw_vertical(
          axis,
          layout.content_box.x,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          layer);
      break;
    case AxisPosition::TOP:
      rc = axis_draw_horizontal(
          axis,
          layout.content_box.y + layout.content_box.h,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          layer);
      break;
    case AxisPosition::BOTTOM:
      rc = axis_draw_horizontal(
          axis,
          layout.content_box.y,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          layer);
      break;
    case AxisPosition::CENTER_HORIZ:
    case AxisPosition::CENTER_VERT:
      return ERROR;
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
  config->scale_layout = bind(&scale_layout_subdivide, _1, _2, 10);
  config->border_style.line_width = from_pt(1);
  config->border_style.color = env.border_color;

  if (elem_name == "chart/axis-top") {
    config->position = AxisPosition::TOP;
  }

  if (elem_name == "chart/axis-right") {
    config->position = AxisPosition::RIGHT;
  }

  if (elem_name == "chart/axis-bottom") {
    config->position = AxisPosition::BOTTOM;
  }

  if (elem_name == "chart/axis-left") {
    config->position = AxisPosition::LEFT;
  }

  switch (config->position) {
    case AxisPosition::TOP:
      config->layout.position = Position::TOP;
      break;
    case AxisPosition::BOTTOM:
      config->layout.position = Position::BOTTOM;
      break;
    case AxisPosition::LEFT:
      config->layout.position = Position::LEFT;
      break;
    case AxisPosition::RIGHT:
      config->layout.position = Position::RIGHT;
      break;
  };

  {
    auto rc = expr_walk_map(expr_next(expr), {
      {"ticks", bind(&scale_configure_layout, _1, &config->scale_layout)},
      {"labels", bind(&format_configure, _1, &config->label_formatter)},
      {"label-font-size", bind(&expr_to_measure, _1, &config->label_font_size)},
      {"label-color", bind(&expr_to_color, _1, &config->label_color)},
      {"label-padding", bind(&expr_to_measure, _1, &config->label_padding)},
      {"limit", bind(&expr_to_float64_opt_pair, _1, &config->scale.min, &config->scale.max)},
      {"limit-min", bind(&expr_to_float64_opt, _1, &config->scale.min)},
      {"limit-max", bind(&expr_to_float64_opt, _1, &config->scale.max)},
      {"scale", bind(&scale_configure_kind, _1, &config->scale)},
      {"scale-padding", bind(&expr_to_float64, _1, &config->scale.padding)},
      {"border", bind(&expr_to_stroke_style, _1, &config->border_style)},
      {"border-color", bind(&expr_to_color, _1, &config->border_style.color)},
      {"border-width", bind(&expr_to_measure, _1, &config->border_style.line_width)},
      {"title", bind(&expr_to_string, _1, &config->title)},
      {"title-font-size", bind(&expr_to_measure, _1, &config->title_font_size)},
      {"title-color", bind(&expr_to_color, _1, &config->title_color)},
      {"title-padding", bind(&expr_to_measure, _1, &config->title_padding)},
      {"title-rotate", bind(&expr_to_float64, _1, &config->title_rotate)},
    });

    if (!rc) {
      return rc;
    }
  }

  if (!config->label_formatter) {
    config->label_formatter = format_decimal_fixed(1);
  }

  switch (config->tick_position) {
    case AxisLabelPosition::OUTSIDE:
      switch (config->position) {
        case AxisPosition::TOP:
          config->tick_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::RIGHT:
          config->tick_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          config->tick_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::LEFT:
          config->tick_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR;
      }
      break;
    case AxisLabelPosition::INSIDE:
      switch (config->position) {
        case AxisPosition::TOP:
          config->tick_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::RIGHT:
          config->tick_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::BOTTOM:
          config->tick_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::LEFT:
          config->tick_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR;
      }
      break;
    default:
      break;
  };

  switch (config->label_position) {
    case AxisLabelPosition::OUTSIDE:
      switch (config->position) {
        case AxisPosition::TOP:
          config->label_position = AxisLabelPosition::TOP;
          config->title_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::RIGHT:
          config->label_position = AxisLabelPosition::RIGHT;
          config->title_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          config->label_position = AxisLabelPosition::BOTTOM;
          config->title_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::LEFT:
          config->label_position = AxisLabelPosition::LEFT;
          config->title_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR;
      }
      break;
    case AxisLabelPosition::INSIDE:
      switch (config->position) {
        case AxisPosition::TOP:
          config->label_position = AxisLabelPosition::BOTTOM;
          config->title_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::RIGHT:
          config->label_position = AxisLabelPosition::LEFT;
          config->title_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::BOTTOM:
          config->label_position = AxisLabelPosition::TOP;
          config->title_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::LEFT:
          config->label_position = AxisLabelPosition::RIGHT;
          config->title_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR;
      }
      break;
    default:
      break;
  };


  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&axis_draw, config, _1, _2);
  (*elem)->size_hint = bind(&axis_calculate_size, config, _1, _2, _3, _4, _5);
  return OK;
}

} // namespace fviz::elements::chart::axis

/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
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
#include "axis.h"
#include <assert.h>
#include <iostream>
#include <source/config_helpers.h>
#include <source/core/scale.h>
#include "source/utils/algo.h"
#include <graphics/text.h>
#include <graphics/layout.h>
#include <graphics/brush.h>

using namespace std::placeholders;

namespace plotfx {

static const double kDefaultLabelPaddingEM = 0.8;
static const double kDefaultLineWidthPT = 1;
static const double kDefaultTickLengthPT = 4;

AxisDefinition::AxisDefinition() :
    position(AxisPosition::BOTTOM),
    label_position(AxisLabelPosition::OUTSIDE),
    tick_position(AxisLabelPosition::INSIDE) {}

ReturnCode parseAxisPosition(
    const std::string& str,
    AxisPosition* value) {
  static const EnumDefinitions<AxisPosition> defs = {
    { "top", AxisPosition::TOP },
    { "right", AxisPosition::RIGHT },
    { "bottom", AxisPosition::BOTTOM },
    { "left", AxisPosition::LEFT },
  };

  return parseEnum(defs, str, value);
}

ReturnCode parseAxisPositionProp(const plist::Property& prop, AxisPosition* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisPosition(prop, value);
}

std::string axis_get_label(
    const AxisDefinition& axis,
    size_t idx,
    double offset) {
  const auto& domain = axis.scale;
  auto value = domain_untranslate(domain, offset);

  if (axis.label_override.size()) {
    if (idx < axis.label_override.size()) {
      return axis.label_override[idx];
    } else {
      return "";
    }
  }

  return axis.label_formatter(std::to_string(value));
}

static Status renderAxisVertical(
    const AxisDefinition& axis_config,
    double x,
    double y0,
    double y1,
    Layer* target) {
  /* compute layout */
  ScaleLayout slayout;
  axis_config.scale_layout(axis_config.scale, &slayout);

  /* draw axis line */
  {
    StrokeStyle style;
    style.color = axis_config.border_color;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(target, {x, y0}, {x, y1}, style);
  }

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
    StrokeStyle style;
    style.color = axis_config.border_color;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(
        target,
        {x, y},
        {x + tick_length * tick_position, y},
        style);
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
      from_em(kDefaultLabelPaddingEM, target->font_size));

  for (size_t i = 0; i < slayout.labels.size(); ++i) {
    auto tick = slayout.labels[i];
    auto label_text = axis_get_label(axis_config, i, tick);

    Point p;
    p.x = x + label_padding * label_position;
    p.y = y0 + (y1 - y0) * (1.0 - tick);

    TextStyle style;
    style.font = axis_config.font;
    style.color = axis_config.text_color;
    style.font_size = axis_config.label_font_size;

    auto ax = label_position > 0 ? HAlign::LEFT : HAlign::RIGHT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); rc != OK) {
      return rc;
    }
  }

  return OK;
}

static Status renderAxisHorizontal(
    const AxisDefinition& axis_config,
    double y,
    double x0,
    double x1,
    Layer* target) {
  /* compute layout */
  ScaleLayout slayout;
  axis_config.scale_layout(axis_config.scale, &slayout);

  /* draw axis line */
  {
    StrokeStyle style;
    style.color = axis_config.border_color;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(target, {x0, y}, {x1, y}, style);
  }

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
    StrokeStyle style;
    style.color = axis_config.border_color;
    style.line_width =  from_pt(kDefaultLineWidthPT, target->dpi);
    strokeLine(
        target,
        {x, y},
        {x, y + tick_length * tick_position},
        style);
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

  auto label_padding = measure_or(
      axis_config.label_padding,
      from_em(kDefaultLabelPaddingEM, target->font_size));

  for (size_t i = 0; i < slayout.labels.size(); ++i) {
    auto tick = slayout.labels[i];
    auto label_text = axis_get_label(axis_config, i, tick);

    Point p;
    p.x = x0 + (x1 - x0) * tick;
    p.y = y + label_padding * label_position;

    TextStyle style;
    style.font = axis_config.font;
    style.color = axis_config.text_color;
    style.font_size = axis_config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = label_position > 0 ? VAlign::TOP : VAlign::BOTTOM;
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, target); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode axis_layout_labels(
    const AxisDefinition& axis,
    const AxisPosition& axis_position,
    const Layer& layer,
    double* margin) {
  /* compute scale layout */
  ScaleLayout slayout;
  axis.scale_layout(axis.scale, &slayout);

  /* compute label size */
  double max = 0;
  for (size_t i = 0; i < slayout.labels.size(); ++i) {
    auto tick = slayout.labels[i];
    auto label_text = axis_get_label(axis, i, tick);

    TextStyle style;
    style.font = axis.font;
    style.font_size = axis.label_font_size;

    Rectangle label_bbox;
    if (auto rc = text::text_measure_span(
          label_text,
          axis.font,
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
        max = std::max(max, label_bbox.h);
        break;
      case AxisPosition::LEFT:
      case AxisPosition::RIGHT:
      case AxisPosition::CENTER_VERT:
        max = std::max(max, label_bbox.w);
        break;
    }
  }

  *margin += max;
  return OK;
}

ReturnCode axis_layout(
    const AxisDefinition& axis,
    const AxisPosition& axis_position,
    const Layer& layer,
    double* margin) {
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

  return OK;
}

ReturnCode axis_layout(
    const Rectangle& parent,
    const AxisDefinition& axis_top,
    const AxisDefinition& axis_right,
    const AxisDefinition& axis_bottom,
    const AxisDefinition& axis_left,
    const Layer& layer,
    Rectangle* bbox) {
  double margins[4] = {0, 0, 0, 0};

  axis_layout(axis_top, AxisPosition::TOP, layer, &margins[0]);
  axis_layout(axis_right, AxisPosition::RIGHT, layer, &margins[1]);
  axis_layout(axis_bottom, AxisPosition::BOTTOM, layer, &margins[2]);
  axis_layout(axis_left, AxisPosition::LEFT, layer, &margins[3]);

  *bbox = layout_margin_box(
      parent,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  return OK;
}

namespace axis {

ReturnCode draw(
    const AxisDefinition& axis,
    const LayoutInfo& layout,
    Layer* frame) {
  const auto& clip = layout.content_box;

  Status rc;
  switch (axis.position) {
    case AxisPosition::LEFT:
      rc = renderAxisVertical(
          axis,
          layout.content_box.x,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          frame);
      break;
    case AxisPosition::RIGHT:
      rc = renderAxisVertical(
          axis,
          layout.content_box.x + layout.content_box.w,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          frame);
      break;
    case AxisPosition::TOP:
      rc = renderAxisHorizontal(
          axis,
          layout.content_box.y,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          frame);
      break;
    case AxisPosition::BOTTOM:
      rc = renderAxisHorizontal(
          axis,
          layout.content_box.y + layout.content_box.h,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          frame);
      break;
    case AxisPosition::CENTER_HORIZ:
    case AxisPosition::CENTER_VERT:
      return ERROR;
  }

  return rc;
}

ReturnCode layout(
    const AxisDefinition& axis,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  double size = 0.0;
  axis_layout(axis, axis.position, layer, &size);

  switch (axis.position) {
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

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    AxisDefinition* config) {
  config->font = env.font;
  config->label_font_size = env.font_size;
  config->border_color = env.border_color;
  config->text_color = env.text_color;

  {
    ParserDefinitions pdefs = {
      {"position", bind(&parseAxisPositionProp, _1, &config->position)},
    };

    if (auto rc = parseAll(plist, pdefs); !rc) {
      return rc;
    }
  }

  switch (config->position) {
    case AxisPosition::TOP:
    case AxisPosition::BOTTOM:
    case AxisPosition::CENTER_HORIZ:
      config->scale = env.scale_x;
      config->scale_layout = env.scale_layout_x;
      break;
    case AxisPosition::LEFT:
    case AxisPosition::RIGHT:
    case AxisPosition::CENTER_VERT:
      config->scale = env.scale_y;
      config->scale_layout = env.scale_layout_y;
      break;
  };

  {
    ParserDefinitions pdefs = {
      {"format", bind(&confgure_format, _1, &config->label_formatter)},
      {"labels", bind(&configure_strings, _1, &config->label_override)},
      {"layout", bind(&configure_scale_layout, _1, &config->scale_layout)},
      {"scale", bind(&domain_configure, _1, &config->scale)},
      {"scale-min", bind(&configure_float_opt, _1, &config->scale.min)},
      {"scale-max", bind(&configure_float_opt, _1, &config->scale.max)},
      {"scale-padding", bind(&configure_float, _1, &config->scale.padding)},
    };

    if (auto rc = parseAll(plist, pdefs); !rc) {
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
          break;
        case AxisPosition::RIGHT:
          config->label_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::BOTTOM:
          config->label_position = AxisLabelPosition::BOTTOM;
          break;
        case AxisPosition::LEFT:
          config->label_position = AxisLabelPosition::LEFT;
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
          break;
        case AxisPosition::RIGHT:
          config->label_position = AxisLabelPosition::LEFT;
          break;
        case AxisPosition::BOTTOM:
          config->label_position = AxisLabelPosition::TOP;
          break;
        case AxisPosition::LEFT:
          config->label_position = AxisLabelPosition::RIGHT;
          break;
        case AxisPosition::CENTER_HORIZ:
        case AxisPosition::CENTER_VERT:
          return ERROR;
      }
      break;
    default:
      break;
  };

  return OK;
}


} // namespace axis
} // namespace plotfx


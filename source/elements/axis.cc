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
#include <source/domain.h>
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
    mode(AxisMode::AUTO),
    position(AxisPosition::BOTTOM),
    label_position(AxisLabelPosition::OUTSIDE),
    tick_position(AxisLabelPosition::INSIDE) {}

ReturnCode parseAxisMode(
    const std::string& str,
    AxisMode* value) {
  static const EnumDefinitions<AxisMode> defs = {
    { "auto", AxisMode::AUTO },
    { "off", AxisMode::OFF },
    { "manual", AxisMode::MANUAL },
  };

  return parseEnum(defs, str, value);
}

ReturnCode parseAxisModeProp(const plist::Property& prop, AxisMode* value) {
  if (!plist::is_value(prop)) {
    return ReturnCode::errorf(
        "EARG",
        "incorrect number of arguments; expected: 1, got: $0",
        prop.size());
  }

  return parseAxisMode(prop, value);
}

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

static Status renderAxisVertical(
    const AxisDefinition& axis_config,
    double x,
    double y0,
    double y1,
    Layer* target) {
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

  for (const auto& tick : axis_config.ticks) {
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

  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;

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

  for (const auto& tick : axis_config.ticks) {
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

  for (const auto& label : axis_config.labels) {
    auto [ tick, label_text ] = label;
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
  double max = 0;

  for (const auto& label : axis.labels) {
    auto [ tick, label_text ] = label;

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
  if (axis.mode == AxisMode::OFF) {
    return OK;
  }

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

ReturnCode axis_place_labels_linear(
    const DomainConfig& domain,
    AxisDefinition* axis,
    double step,
    std::optional<double> align) {
  axis->ticks.clear();
  axis->labels.clear();

  auto begin = std::max(align.value_or(domain_min(domain)), domain_min(domain));
  auto end = domain_max(domain);

  for (auto v = begin; v <= end; v += step) {
    auto vp = domain_translate(domain, v);
    axis->ticks.emplace_back(vp);
    axis->labels.emplace_back(vp, axis->label_formatter(std::to_string(v)));
  }

  return OK;
}

ReturnCode axis_place_labels_subdivide(
    const DomainConfig& domain,
    AxisDefinition* axis,
    uint32_t divisions) {
  axis->ticks.clear();
  axis->labels.clear();

  for (size_t i = 0; i < divisions; ++i) {
    axis->ticks.emplace_back((1.0f / (divisions - 1)) * i);
  }

  auto tick_values = domain_untranslate(domain, axis->ticks);
  for (size_t i = 0; i < divisions; ++i) {
    axis->labels.emplace_back(
        axis->ticks[i],
        axis->label_formatter(tick_values[i]));
  }

  return OK;
}

ReturnCode axis_place_labels_discrete(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  auto range = domain_max(domain) + 1;

  axis->labels.clear();
  axis->ticks.clear();
  axis->ticks.push_back(0.0f);

  std::vector<double> label_positions;
  for (size_t i = 0; i < range; ++i) {
    auto o = (1.0f / range) * (i + 1);
    label_positions.push_back(o - 0.5 / range);
    axis->ticks.push_back(o);
  }

  auto label_values = domain_untranslate(domain, label_positions);
  for (size_t i = 0; i < label_positions.size(); ++i) {
    axis->labels.emplace_back(
        label_positions[i],
        axis->label_formatter(label_values[i]));
  }

  return OK;
}

ReturnCode axis_place_labels_default(
    const DomainConfig& domain,
    AxisDefinition* axis) {
  if (domain.kind == DomainKind::DISCRETE) {
    return axis_place_labels_discrete(domain, axis);
  }

  return axis_place_labels_subdivide(domain, axis, 8); // TODO: automatically choose a good value
}


ReturnCode configure_label_placement_linear(
    const plist::Property& prop,
    AxisLabelPlacement* label_placement) {
  double step = 0;
  std::optional<double> align;
  switch (prop.size()) {
    case 0:
      step = 1; // TODO: automatically choose a good value
      break;
    case 1:
    default:
      try {
        step = std::stod(prop[0]);
        break;
      } catch (... ) {
        return ERROR;
      }
  }

  for (size_t i = 1; i < prop.size(); ++i) {
    if (plist::is_tuple(prop[i]) &&
        prop[i].size() == 2 &&
        prop[i][0].value == "align") {
      try {
        align = std::stod(prop[i][1].value);
        break;
      } catch (... ) {
        return ERROR;
      }

      continue;
    }
  }

  *label_placement = bind(
      &axis_place_labels_linear,
      _1,
      _2,
      step,
      align);

  return OK;
}

ReturnCode configure_label_placement_subdivide(
    const plist::Property& prop,
    AxisLabelPlacement* label_placement) {
  double subdivisions = 0;
  switch (prop.size()) {
    case 0:
      subdivisions = 8; // TODO: automatically choose a good value
      break;
    case 1:
      try {
        subdivisions = std::stod(prop[0]);
        break;
      } catch (... ) {
        return ERROR;
      }
    default:
      return ERROR;
  }

  *label_placement = bind(
      &axis_place_labels_subdivide,
      _1,
      _2,
      subdivisions);

  return OK;
}

ReturnCode axis_configure_label_placement(
    const plist::Property& prop,
    AxisLabelPlacement* label_placement) {
  if (plist::is_value(prop, "linear") ||
      plist::is_enum(prop, "linear")) {
    return configure_label_placement_linear(prop, label_placement);
  }

  if (plist::is_value(prop, "subdivide") ||
      plist::is_enum(prop, "subdivide")) {
    return configure_label_placement_subdivide(prop, label_placement);
  }

  if (plist::is_value(prop, "discrete") ||
      plist::is_enum(prop, "discrete")) {
    *label_placement = bind(
        &axis_place_labels_discrete,
        _1,
        _2);

    return OK;
  }

  return ReturnCode::errorf(
      "EARG",
      "invalid value '$0', expected one of: \n"
      "  - linear\n"
      "  - subdivide\n"
      "  - discrete\n",
      prop.value);
}

namespace axis {

ReturnCode draw(
    const AxisDefinition& axis,
    const LayoutInfo& layout,
    Layer* frame) {
  const auto& clip = layout.element_box;

  switch (axis.mode) {
    case AxisMode::OFF:
      return OK;
    default:
      break;
  };

  Status rc;
  switch (axis.position) {
    case AxisPosition::LEFT:
      rc = renderAxisVertical(
          axis,
          layout.element_box.x,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          frame);
      break;
    case AxisPosition::RIGHT:
      rc = renderAxisVertical(
          axis,
          layout.element_box.x + layout.element_box.w,
          layout.content_box.y,
          layout.content_box.y + layout.content_box.h,
          frame);
      break;
    case AxisPosition::TOP:
      rc = renderAxisHorizontal(
          axis,
          layout.element_box.y,
          layout.content_box.x,
          layout.content_box.x + layout.content_box.w,
          frame);
      break;
    case AxisPosition::BOTTOM:
      rc = renderAxisHorizontal(
          axis,
          layout.element_box.y + layout.element_box.h,
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
    LayoutInfo* layout) {
  double margins[4] = {0, 0, 0, 0};

  switch (axis.position) {
    case AxisPosition::TOP:
      axis_layout(axis, axis.position, layer, &margins[0]);
      break;
    case AxisPosition::RIGHT:
      axis_layout(axis, axis.position, layer, &margins[1]);
      break;
    case AxisPosition::BOTTOM:
      axis_layout(axis, axis.position, layer, &margins[2]);
      break;
    case AxisPosition::LEFT:
      axis_layout(axis, axis.position, layer, &margins[3]);
      break;
  }

  layout->content_box = layout_margin_box(
      layout->content_box,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  layout->element_box = layout->content_box;

  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const DataContext& data,
    const Document& doc,
    const Environment& env,
    AxisDefinition* config) {
  config->font = doc.font_sans;
  config->label_font_size = doc.font_size;
  config->border_color = doc.border_color;
  config->text_color = doc.text_color;

  {
    static const ParserDefinitions pdefs = {
      {"position", bind(&parseAxisPositionProp, _1, &config->position)},
      {"layout", bind(&axis_configure_label_placement, _1, &config->label_placement)},
      {"format", bind(&confgure_format, _1, &config->label_formatter)},

    };

    if (auto rc = parseAll(plist, pdefs); !rc) {
      return rc;
    }
  }

  DomainConfig domain;
  switch (config->position) {
    case AxisPosition::TOP:
    case AxisPosition::BOTTOM:
    case AxisPosition::CENTER_HORIZ:
      domain = env.scale_x;
      break;
    case AxisPosition::LEFT:
    case AxisPosition::RIGHT:
    case AxisPosition::CENTER_VERT:
      domain = env.scale_y;
      break;
  };

  {
    static const ParserDefinitions pdefs = {
      {"scale", bind(&domain_configure, _1, &domain)},
      {"scale-min", bind(&configure_float_opt, _1, &domain.min)},
      {"scale-max", bind(&configure_float_opt, _1, &domain.max)},
      {"scale-padding", bind(&configure_float, _1, &domain.padding)},
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

  if (config->label_placement) {
    if (auto rc = config->label_placement(domain, config); !rc) {
      return rc;
    }
  } else {
    if (auto rc = axis_place_labels_default(domain, config); !rc) {
      return rc;
    }
  }

  return OK;
}


} // namespace axis
} // namespace plotfx


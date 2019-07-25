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
#include "item.h"

#include "data.h"
#include "environment.h"
#include "marker.h"
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

namespace fviz::elements::legend::item {

struct LegendItemElem {
  std::string label;
  HAlign label_align;
  Measure label_margin;
  Color label_color;
  FontInfo label_font;
  Measure label_font_size;
  Marker marker;
  HAlign marker_align;
  Measure marker_margin;
  Color marker_color;
  Measure marker_size;
};

void normalize(
    std::shared_ptr<LegendItemElem> config,
    const Layer& layer) {
  convert_unit_typographic(
      layer.dpi,
      layer.font_size,
      &config->label_font_size);

  convert_unit_typographic(
      layer.dpi,
      config->label_font_size,
      &config->label_margin);

  convert_unit_typographic(
      layer.dpi,
      layer.font_size,
      &config->marker_size);

  convert_unit_typographic(
      layer.dpi,
      config->marker_size,
      &config->marker_margin);
}

ReturnCode layout(
    std::shared_ptr<LegendItemElem> config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  normalize(config, layer);

  /* add label extents */
  TextStyle style;
  style.font = config->label_font;
  style.font_size = config->label_font_size;

  Rectangle label_bbox;
  if (auto rc = text::text_measure_span(
        config->label,
        config->label_font,
        config->label_font_size,
        layer.dpi,
        layer.text_shaper.get(),
        &label_bbox);
       rc != Status::OK) {
    return rc;
  }

  *min_height += label_bbox.h;
  *min_width += label_bbox.w;

  /* add label margin */
  *min_width += config->label_margin;

  return OK;
}

ReturnCode draw_label(
    const LegendItemElem& config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& bbox = layout.content_box;
  const auto& text = config.label;

  TextStyle style;
  style.font = config.label_font;
  style.color = config.label_color;
  style.font_size = config.label_font_size;

  Point p;
  auto ax = HAlign::CENTER;
  auto ay = VAlign::CENTER;

  switch (config.label_align) {
    case HAlign::LEFT:
      ax = HAlign::LEFT;
      p.x = bbox.x + config.label_margin;
      p.y = bbox.y + bbox.h * 0.5;
      break;
    case HAlign::RIGHT:
      ax = HAlign::RIGHT;
      p.x = bbox.x + bbox.w - config.label_margin;
      p.y = bbox.y + bbox.h * 0.5;
      break;
  }

  if (auto rc = drawTextLabel(text, p, ax, ay, style, layer); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode draw_marker(
    const LegendItemElem& config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& bbox = layout.content_box;

  Point p;
  switch (config.marker_align) {
    case HAlign::LEFT:
      p.x = bbox.x + config.marker_margin;
      p.y = bbox.y + bbox.h * 0.5;
      break;
    case HAlign::RIGHT:
      p.x = bbox.x + bbox.w - config.marker_margin;
      p.y = bbox.y + bbox.h * 0.5;
      break;
  }

  const auto& s = config.marker_size;
  const auto& c = config.marker_color;
  if (auto rc = config.marker(p, s, c, layer); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode draw(
    std::shared_ptr<LegendItemElem> config,
    const LayoutInfo& layout,
    Layer* layer) {
  /* convert units */
  normalize(config, *layer);

  /* draw label */
  if (auto rc = draw_label(*config, layout, layer); !rc) {
    return rc;
  }

  /* draw marker */
  if (auto rc = draw_marker(*config, layout, layer); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* inherit defaults */
  auto config = std::make_shared<LegendItemElem>();
  config->label_align = HAlign::LEFT;
  config->label_margin = from_em(1.1);
  config->label_font = env.font;
  config->label_font_size = env.font_size;
  config->label_color = env.text_color;
  config->marker = marker_create_disk();
  config->marker_align = HAlign::LEFT;
  config->marker_margin = from_em(0.2);
  config->marker_size = env.font_size;
  config->marker_size.value *= 0.75;
  config->marker_color = env.text_color;

  /* parse exprerties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"label", bind(&expr_to_string, _1, &config->label)},
    {
      "label-align",
      expr_to_enum_fn<HAlign>(&config->label_align, {
        {"left", HAlign::LEFT},
        {"right", HAlign::RIGHT},
      })
    },
    {"label-margin", bind(&expr_to_measure, _1, &config->label_margin)},
    {"label-color", bind(&expr_to_color, _1, &config->label_color)},
    {"label-font-size", bind(&expr_to_measure, _1, &config->label_font_size)},
    {"marker-shape", bind(&marker_configure, _1, &config->marker)},
    {
      "marker-align",
      expr_to_enum_fn<HAlign>(&config->marker_align, {
        {"left", HAlign::LEFT},
        {"right", HAlign::RIGHT},
      })
    },
    {"marker-margin", bind(&expr_to_measure, _1, &config->marker_margin)},
    {"marker-color", bind(&expr_to_color, _1, &config->marker_color)},
    {"marker-size", bind(&expr_to_measure, _1, &config->marker_size)},
    {"color", bind(&expr_to_color, _1, &config->marker_color)},
  });

  if (!config_rc) {
    return config_rc;
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  (*elem)->size_hint = bind(&layout, config, _1, _2, _3, _4, _5);
  return OK;
}

} // namespace fviz::elements::legend::item


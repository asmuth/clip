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
#include "legend_item.h"

#include "data.h"
#include "context.h"
#include "layout.h"
#include "scale.h"
#include "sexpr.h"
#include "color_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

void legend_item_normalize(
    Context* ctx,
    LegendItem* config) {
  //convert_unit_typographic(
  //    layer_get_dpi(ctx),
  //    layer_get_font_size(ctx),
  //    &config->label_font_size);

  //convert_unit_typographic(
  //    layer_get_dpi(ctx),
  //    config->label_font_size,
  //    &config->label_margin);

  //convert_unit_typographic(
  //    layer_get_dpi(ctx),
  //    layer_get_font_size(ctx),
  //    &config->marker_size);

  //convert_unit_typographic(
  //    layer_get_dpi(ctx),
  //    config->marker_size,
  //    &config->marker_margin);
}

ReturnCode legend_item_calculate_size(
    Context* ctx,
    LegendItem* config,
    double* min_width,
    double* min_height) {
  legend_item_normalize(ctx, config);

  /* add label extents */
  TextStyle style;
  style.font = config->label_font;
  style.font_size = config->label_font_size;

  Rectangle label_bbox;
  if (auto rc = text_measure_label(
        config->label,
        TextDirection::LTR,
        config->label_font,
        config->label_font_size,
        layer_get_dpi(ctx),
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
    Context* ctx,
    const LegendItem& config,
    const Rectangle& bbox) {
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

  if (auto rc = draw_text(ctx, text, p, ax, ay, style); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode draw_marker(
    Context* ctx,
    const LegendItem& config,
    const Rectangle& bbox) {
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
  if (auto rc = config.marker(ctx, p, s, c); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode legend_item_draw(
    Context* ctx,
    LegendItem* config,
    const Rectangle& bbox) {
  /* convert units */
  legend_item_normalize(ctx, config);

  /* draw label */
  if (auto rc = draw_label(ctx, *config, bbox); !rc) {
    return rc;
  }

  /* draw marker */
  if (auto rc = draw_marker(ctx, *config, bbox); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode legend_item_configure(
    Context* ctx,
    const Expr* expr,
    LegendItem* config) {
  const auto& layer = *layer_get(ctx);

  /* inherit defaults */
  config->label_align = HAlign::LEFT;
  config->label_margin = from_em(1.1);
  config->label_font = layer_get_font(ctx);
  config->label_font_size = layer_get_font_size(ctx);
  config->label_color = layer_get(ctx)->text_color;
  config->marker = marker_create_disk();
  config->marker_align = HAlign::LEFT;
  config->marker_margin = from_em(0.2);
  config->marker_size = layer_get_font_size(ctx);
  config->marker_size.value *= 0.75;
  config->marker_color = layer_get(ctx)->text_color;

  /* parse exprerties */
  auto config_rc = expr_walk_map(expr, {
    {"font", expr_call_string_fn(std::bind(&font_load_best, _1, &config->label_font))},
    {"label", std::bind(&expr_to_string, _1, &config->label)},
    {
      "label-align",
      expr_to_enum_fn<HAlign>(&config->label_align, {
        {"left", HAlign::LEFT},
        {"right", HAlign::RIGHT},
      })
    },
    {"label-margin", std::bind(&measure_read, _1, &config->label_margin)},
    {"label-color", std::bind(&color_read, ctx, _1, &config->label_color)},
    {"label-font", expr_call_string_fn(std::bind(&font_load_best, _1, &config->label_font))},
    {"label-font-size", std::bind(&expr_to_font_size, _1, layer, &config->label_font_size)},
    {"marker-shape", std::bind(&marker_configure, _1, &config->marker)},
    {
      "marker-align",
      expr_to_enum_fn<HAlign>(&config->marker_align, {
        {"left", HAlign::LEFT},
        {"right", HAlign::RIGHT},
      })
    },
    {"marker-margin", std::bind(&measure_read, _1, &config->marker_margin)},
    {"marker-color", std::bind(&color_read, ctx, _1, &config->marker_color)},
    {"marker-size", std::bind(&expr_to_font_size, _1, layer, &config->marker_size)},
    {"color", std::bind(&color_read, ctx, _1, &config->marker_color)},
  });

  if (!config_rc) {
    return config_rc;
  }

  return OK;
}

} // namespace clip::plotgen


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
#include "lines.h"

#include "data.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "scale.h"
#include "layout.h"
#include "color_reader.h"
#include "marker.h"
#include "style.h"
#include "style_reader.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

static const double kDefaultLineWidthPT = 1.5;
static const double kDefaultLabelPaddingEM = 0.4;

struct PlotLinesConfig {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<DataGroup> groups;
  StrokeStyle stroke_style;
  Measure marker_size;
  Marker marker_shape;
  Color marker_color;
  std::vector<std::string> labels;
  FontInfo label_font;
  Color label_color;
  Measure label_padding;
  Measure label_font_size;
};

ReturnCode lines_draw(
    Context* ctx,
    PlotConfig* plot,
    PlotLinesConfig* config) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));
  /* convert units */
  convert_units(
      {
        std::bind(&convert_unit_typographic, layer_get_dpi(ctx), layer_get_font_size(ctx), _1),
        std::bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        std::bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        std::bind(&convert_unit_typographic, layer_get_dpi(ctx), layer_get_font_size(ctx), _1),
        std::bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        std::bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  MeasureConv conv;
  conv.dpi = layer_get_dpi(ctx);
  conv.font_size = layer_get_font_size(ctx);
  conv.parent_size = layer_get_font_size(ctx);

  measure_normalize(conv, &config->stroke_style.line_width);
  measure_normalize(conv, &config->label_padding);
  measure_normalize(conv, &config->label_font_size);
  measure_normalize(conv, &config->marker_size);

  /* draw lines */
  for (const auto& group : config->groups) {
    if (group.index.empty()) {
      continue;
    }

    Path path;
    for (auto i : group.index) {
      auto sx = clip.x + config->x[i];
      auto sy = clip.y + config->y[i];

      if (i == group.index[0]) {
        path.moveTo(sx, sy);
      } else {
        path.lineTo(sx, sy);
      }
    }

    DrawCommand elem;
    elem.path = path;
    elem.stroke_style = config->stroke_style;
    draw_shape(ctx, elem);
  }

  /* draw markers */
  if (config->marker_size > 0) {
    for (size_t i = 0; i < config->x.size(); ++i) {
      auto sx = clip.x + config->x[i];
      auto sy = clip.y + config->y[i];

      const auto& shape = config->marker_shape;
      const auto& color = config->marker_color;
      auto size = config->marker_size;

      if (auto rc = shape(ctx, Point(sx, sy), size, color); !rc) {
        return rc;
      }
    }
  }

  /* draw labels */
  for (size_t i = 0; i < config->labels.size(); ++i) {
    const auto& label_text = config->labels[i];

    auto label_offset  = config->marker_size;
    auto label_padding = label_offset + measure_or(
        config->label_padding,
        from_em(kDefaultLabelPaddingEM, config->label_font_size));

    Point p(
        clip.x + config->x[i],
        clip.y + config->y[i] + label_padding);

    TextStyle style;
    style.font = config->label_font;
    style.color = config->label_color;
    style.font_size = config->label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = draw_text(ctx, label_text, p, ax, ay, style); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode lines_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotLinesConfig* c,
    const Expr* expr) {
  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->label_font = layer_get_font(ctx);
  c->label_font_size = layer_get_font_size(ctx);
  c->stroke_style.color = layer_get(ctx)->foreground_color;
  c->stroke_style.line_width = from_pt(kDefaultLineWidthPT);
  c->marker_shape = marker_create_disk();
  c->marker_color = layer_get(ctx)->foreground_color;

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_y;

  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data-x", std::bind(&data_load_strings, _1, &data_x)},
    {"data-y", std::bind(&data_load_strings, _1, &data_y)},
    {"limit-x", std::bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", std::bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", std::bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", std::bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", std::bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", std::bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"scale-x", std::bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", std::bind(&scale_configure_kind, _1, &c->scale_y)},
    {"scale-x-padding", std::bind(&expr_to_float64, _1, &c->scale_x.padding)},
    {"scale-y-padding", std::bind(&expr_to_float64, _1, &c->scale_y.padding)},
    {
      "color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &c->stroke_style.color),
        std::bind(&color_read, ctx, _1, &c->marker_color),
      })
    },
    {"stroke-width", std::bind(&measure_read, _1, &c->stroke_style.line_width)},
    {"stroke-style", std::bind(&stroke_style_read, ctx, _1, &c->stroke_style)},
    {"stroke-color", std::bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"marker-size", std::bind(&measure_read, _1, &c->marker_size)},
    {"marker-shape", std::bind(&marker_configure, _1, &c->marker_shape)},
    {"marker-color", std::bind(&color_read, ctx, _1, &c->marker_color)},
    {"labels", std::bind(&data_load_strings, _1, &c->labels)},
    {"label-font-size", std::bind(&measure_read, _1, &c->label_font_size)},
    {"label-color", std::bind(&color_read, ctx, _1, &c->label_color)},
    {"label-padding", std::bind(&measure_read, _1, &c->label_padding)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* scale configuration */
  if (auto rc = data_to_measures(data_x, c->scale_x, &c->x); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_y, c->scale_y, &c->y); !rc){
    return rc;
  }

  for (const auto& v : c->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
  }

  /* check configuraton */
  if (c->x.size() != c->y.size()) {
    return error(
        ERROR,
        "The length of the 'data-x' and 'data-y' properties must be equal");
  }

  if (c->x.empty() || c->y.empty()) {
    return error(
        ERROR,
        "The dataset is empty");
  }

  /* group data */
  {
    DataGroup g;
    g.index = std::vector<size_t>(c->x.size());
    std::iota(g.index.begin(), g.index.end(), 0);
    c->groups.emplace_back(g);
  }

  return OK;
}

ReturnCode lines_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotLinesConfig conf;

  if (auto rc = lines_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  return lines_draw(ctx, plot, &conf);
}

ReturnCode lines_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotLinesConfig conf;
  return lines_configure(ctx, plot, &conf, expr);
}

} // namespace clip::plotgen


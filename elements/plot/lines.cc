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
#include "environment.h"
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

namespace clip::elements::plot::lines {

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

ReturnCode draw(
    std::shared_ptr<PlotLinesConfig> config,
    const LayoutInfo& layout,
    Page* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  MeasureConv conv;
  conv.dpi = layer->dpi;
  conv.font_size = layer->font_size;
  conv.parent_size = layer->font_size;

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

    strokePath(layer, clip, path, config->stroke_style);
  }

  /* draw markers */
  if (config->marker_size > 0) {
    for (size_t i = 0; i < config->x.size(); ++i) {
      auto sx = clip.x + config->x[i];
      auto sy = clip.y + config->y[i];

      const auto& shape = config->marker_shape;
      const auto& color = config->marker_color;
      auto size = config->marker_size;

      if (auto rc = shape(Point(sx, sy), size, color, layer); !rc) {
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
    if (auto rc = drawTextLabel(label_text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* set defaults from environment */
  auto c = std::make_shared<PlotLinesConfig>();
  c->label_font = env.font;
  c->label_font_size = env.font_size;
  c->stroke_style.color = env.foreground_color;
  c->stroke_style.line_width = from_pt(kDefaultLineWidthPT);
  c->marker_shape = marker_create_disk();
  c->marker_color = env.foreground_color;

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_y;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"data-x", bind(&data_load_strings, _1, &data_x)},
    {"data-y", bind(&data_load_strings, _1, &data_y)},
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &c->scale_x.min, &c->scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &c->scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &c->scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &c->scale_y.min, &c->scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &c->scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &c->scale_y.max)},
    {"scale-x", bind(&scale_configure_kind, _1, &c->scale_x)},
    {"scale-y", bind(&scale_configure_kind, _1, &c->scale_y)},
    {"scale-x-padding", bind(&expr_to_float64, _1, &c->scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &c->scale_y.padding)},
    {
      "color",
      expr_calln_fn({
        bind(&color_read, env, _1, &c->stroke_style.color),
        bind(&color_read, env, _1, &c->marker_color),
      })
    },
    {"stroke-width", bind(&measure_read, _1, &c->stroke_style.line_width)},
    {"stroke-style", bind(&stroke_style_read, env, _1, &c->stroke_style)},
    {"stroke-color", bind(&color_read, env, _1, &c->stroke_style.color)},
    {"marker-size", bind(&measure_read, _1, &c->marker_size)},
    {"marker-shape", bind(&marker_configure, _1, &c->marker_shape)},
    {"marker-color", bind(&color_read, env, _1, &c->marker_color)},
    {"labels", bind(&data_load_strings, _1, &c->labels)},
    {"label-font-size", bind(&measure_read, _1, &c->label_font_size)},
    {"label-color", bind(&color_read, env, _1, &c->label_color)},
    {"label-padding", bind(&measure_read, _1, &c->label_padding)},
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
        "the length of the 'data-x' and 'data-y' properties must be equal");
  }

  /* group data */
  {
    DataGroup g;
    g.index = std::vector<size_t>(c->x.size());
    std::iota(g.index.begin(), g.index.end(), 0);
    c->groups.emplace_back(g);
  }

  /* return element */
  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, c, _1, _2);
  return OK;
}

} // namespace clip::elements::plot::lines


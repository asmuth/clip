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
#include "labels.h"

#include "data.h"
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

namespace fviz::elements::chart::labels {

static const double kDefaultLabelPaddingEM = 0.8;

struct PlotLabelsConfig {
  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<std::string> labels;
  FontInfo label_font;
  Measure label_padding;
  Measure label_font_size;
  Color label_color;
};

ReturnCode draw(
    std::shared_ptr<PlotLabelsConfig> config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  /* draw labels */
  for (size_t i = 0; i < config->labels.size(); ++i) {
    const auto& label_text = config->labels[i];
    auto label_padding = measure_or(
        config->label_padding,
        from_em(kDefaultLabelPaddingEM, config->label_font_size));

    Point p(
        clip.x + config->x[i] * clip.w,
        clip.y + (1.0 - config->y[i]) * clip.h - label_padding);

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
  auto config = std::make_shared<PlotLabelsConfig>();
  config->label_font = env.font;
  config->label_font_size = env.font_size;

  /* parse properties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"xdata", bind(&data_load, _1, &config->x)},
    {"ydata", bind(&data_load, _1, &config->y)},
    {"xmin", bind(&expr_to_float64_opt, _1, &config->scale_x.min)},
    {"xmax", bind(&expr_to_float64_opt, _1, &config->scale_x.max)},
    {"xscale", bind(&scale_configure_kind, _1, &config->scale_x)},
    {"xscale-padding", bind(&expr_to_float64, _1, &config->scale_x.padding)},
    {"ymin", bind(&expr_to_float64_opt, _1, &config->scale_y.min)},
    {"ymax", bind(&expr_to_float64_opt, _1, &config->scale_y.max)},
    {"yscale", bind(&scale_configure_kind, _1, &config->scale_y)},
    {"yscale-padding", bind(&expr_to_float64, _1, &config->scale_y.padding)},
    {"labels", bind(&data_load_strings, _1, &config->labels)},
    {"label-font-size", bind(&expr_to_measure, _1, &config->label_font_size)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* check configuraton */
  if (config->x.size() != config->y.size()) {
    return error(
        ERROR,
        "the length of the 'xs' and 'ys' properties must be equal");
  }

  /* scale autoconfig */
  for (const auto& v : config->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &config->scale_x);
    }
  }

  for (const auto& v : config->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &config->scale_y);
    }
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::labels


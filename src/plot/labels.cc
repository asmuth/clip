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
#include "labels.h"

#include "data.h"
#include "context.h"
#include "layout.h"
#include "scale.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "color_reader.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

static const double kDefaultLabelPaddingEM = 0.8;

struct PlotLabelsConfig {
  DataBuffer x;
  DataBuffer y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<std::string> labels;
  FontInfo label_font;
  Measure label_padding;
  Number label_font_size;
  Color label_color;
};

ReturnCode labels_draw(
    Context* ctx,
    PlotConfig* plot,
    PlotLabelsConfig* config) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));

  /* transform data */
  std::vector<double> xs;
  if (auto rc = scale_translatev(config->scale_x, config->x, &xs); !rc) {
    return rc;
  }

  std::vector<double> ys;
  if (auto rc = scale_translatev(config->scale_y, config->y, &ys); !rc) {
    return rc;
  }

  /* draw labels */
  for (size_t i = 0; i < config->labels.size(); ++i) {
    const auto& label_text = config->labels[i];
    auto label_padding = measure_or(
        config->label_padding,
        from_em(kDefaultLabelPaddingEM, config->label_font_size.value));

    Point p(
        clip.x + xs[i] * clip.w,
        clip.y + (1.0 - ys[i]) * clip.h - label_padding);

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

ReturnCode labels_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotLabelsConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->label_font = layer_get_font(ctx);
  c->label_font_size = layer_get_font_size(ctx);

  /* parse properties */
  std::string data_ref;
  std::string data_format;
  ExprStorage data_x;
  ExprStorage data_y;

  auto config_rc = expr_walk_map(expr, {
    {"data", std::bind(&expr_to_string, _1, &data_ref)},
    {"data-format", std::bind(&expr_to_string, _1, &data_format)},
    {"data-x", std::bind(&expr_to_copy, _1, &data_x)},
    {"data-y", std::bind(&expr_to_copy, _1, &data_y)},
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
    {"labels", std::bind(&data_load_strings, _1, &c->labels)},
    {"label-font", expr_call_string_fn(std::bind(&font_load_best, _1, &c->label_font))},
    {"label-font-size", std::bind(&expr_to_font_size, _1, layer, &c->label_font_size)},
    {"label-color", std::bind(&color_read, ctx, _1, &c->label_color)},
    {"label-padding", std::bind(&measure_read, _1, &c->label_padding)},
    {"font", expr_call_string_fn(std::bind(&font_load_best, _1, &c->label_font))},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* load data files */
  auto data_rc = data_load_points2(
      data_ref,
      data_format,
      data_x.get(),
      data_y.get(),
      &c->x,
      &c->y);

  if (!data_rc) {
    return data_rc;
  }

  /* check configuration */
  if (databuf_len(c->x) != databuf_len(c->y)) {
    return error(ERROR, "The length of the 'data-x' and 'data-y' lists must be equal");
  }

  return OK;
}

ReturnCode labels_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotLabelsConfig conf;

  if (auto rc = labels_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  return labels_draw(ctx, plot, &conf);
}

ReturnCode labels_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotLabelsConfig conf;
  if (auto rc = labels_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_x, conf.x); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_y, conf.y); !rc) {
    return rc;
  }

  return OK;
}

} // namespace clip::plotgen


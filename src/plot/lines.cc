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
static const double kDefaultLabelPaddingEM = .4;

struct PlotLinesConfig {
  DataBuffer x;
  DataBuffer y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::vector<size_t> groups;
  StrokeStyle stroke_style;
  Number marker_size;
  Marker marker_shape;
  Color marker_color;
  std::vector<std::string> labels;
  FontInfo label_font;
  Color label_color;
  Number label_padding;
  Number label_font_size;
};

ReturnCode lines_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotLinesConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->label_font = layer_get_font(ctx);
  c->label_font_size = layer_get_font_size(ctx);
  c->stroke_style.color = layer_get(ctx)->foreground_color;
  c->stroke_style.line_width = unit_from_pt(kDefaultLineWidthPT, layer_get_dpi(ctx));
  c->marker_shape = marker_create_disk();
  c->marker_color = layer_get(ctx)->foreground_color;

  /* parse properties */
  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data", std::bind(&data_load_polylines2, _1, &c->x, &c->y, &c->groups)},
    {"data-x", std::bind(&data_load_simple, _1, &c->x)},
    {"data-y", std::bind(&data_load_simple, _1, &c->y)},
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
    {"stroke-width", std::bind(&expr_to_size, _1, layer, &c->stroke_style.line_width)},
    {"stroke-style", std::bind(&stroke_style_read, ctx, _1, &c->stroke_style)},
    {"stroke-color", std::bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"marker-size", std::bind(&expr_to_size, _1, layer, &c->marker_size)},
    {"marker-shape", std::bind(&marker_configure, _1, &c->marker_shape)},
    {"marker-color", std::bind(&color_read, ctx, _1, &c->marker_color)},
    {"labels", std::bind(&data_load_strings, _1, &c->labels)},
    {"label-font-size", std::bind(&expr_to_font_size, _1, layer, &c->label_font_size)},
    {"label-color", std::bind(&color_read, ctx, _1, &c->label_color)},
    {"label-padding", std::bind(&expr_to_size, _1, layer, &c->label_padding)},
  });

  if (!config_rc) {
    return config_rc;
  }

  if (databuf_len(c->x) != databuf_len(c->y)) {
    return error(ERROR, "The length of the 'data-x' and 'data-y' lists must be equal");
  }

  if (!databuf_len(c->x) || !databuf_len(c->y)) {
    return error(ERROR, "The dataset is empty");
  }

  return OK;
}

ReturnCode lines_draw(
    Context* ctx,
    const PlotConfig* plot,
    const PlotLinesConfig* config) {
  const auto& groups = config->groups;
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

  /* draw lines */
  for (size_t n = 0; n <= groups.size(); ++n) {
    auto line_begin = n > 0 ? groups[n - 1] : 0;
    auto line_end = n < groups.size() ? groups[n] : std::min(xs.size(), ys.size());

    Path path;
    for (size_t i = line_begin; i < line_end; ++i) {
      auto sx = clip.x + xs[i] * clip.w;
      auto sy = clip.y + ys[i] * clip.h;

      if (i == line_begin) {
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
  if (config->marker_size.value > 0) {
    for (size_t i = 0; i < xs.size(); ++i) {
      auto sx = clip.x + xs[i] * clip.w;
      auto sy = clip.y + ys[i] * clip.h;

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
    auto label_padding = config->label_padding.value
        ? config->label_padding
        : unit_from_em(kDefaultLabelPaddingEM, config->label_font_size);

    Point p(
        clip.x + xs[i] * clip.w,
        clip.y + ys[i] * clip.h + label_offset.value + label_padding.value);

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
  if (auto rc = lines_configure(ctx, plot, &conf, expr); !rc) {
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


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
#include "points.h"
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "context.h"
#include "color_reader.h"
#include "typographic_map.h"
#include "typographic_reader.h"
#include "layout.h"
#include "marker.h"
#include "scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

static const double kDefaultPointSizePT = 4;
static const double kDefaultLabelPaddingEM = 0.2;

struct PlotPointsConfig {
  DataBuffer x;
  DataBuffer y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  Color color;
  std::vector<Color> colors;
  Number size;
  std::vector<Measure> sizes;
  Marker shape;
  std::vector<Marker> shapes;
  std::vector<std::string> labels;
  FontInfo label_font;
  Number label_padding;
  Number label_font_size;
  Color label_color;
};

ReturnCode points_draw(
    Context* ctx,
    PlotConfig* plot,
    PlotPointsConfig* config) {
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

  /* draw markers */
  for (size_t i = 0; i < xs.size(); ++i) {
    auto sx = clip.x + xs[i] * clip.w;
    auto sy = clip.y + ys[i] * clip.h;

    const auto& color = config->colors.empty()
        ? config->color
        : config->colors[i % config->colors.size()];

    auto size = config->sizes.empty()
        ? config->size
        : Number(config->sizes[i % config->sizes.size()]);

    auto shape = config->shapes.empty()
        ? config->shape
        : config->shapes[i % config->shapes.size()];

    if (auto rc = shape(ctx, Point(sx, sy), size, color); !rc) {
      return rc;
    }
  }

  /* draw labels */
  for (size_t i = 0; i < config->labels.size(); ++i) {
    const auto& label_text = config->labels[i];

    auto size = config->sizes.empty()
        ? 0
        : config->sizes[i % config->sizes.size()].value;

    auto label_padding = size * 0.5 + (
        config->label_padding.value
            ? config->label_padding.value
            : config->label_font_size.value * .2);

    Point p(
        clip.x + xs[i] * clip.w,
        clip.y + ys[i] * clip.h + label_padding);

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

ReturnCode points_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotPointsConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->color = layer_get(ctx)->foreground_color;
  c->size = unit_from_pt(kDefaultPointSizePT, layer_get_dpi(ctx));
  c->shape = marker_create_disk();
  c->label_font = layer_get_font(ctx);
  c->label_font_size = layer_get_font_size(ctx);

  /* parse properties */
  std::string data_ref;
  std::string data_format;
  ExprStorage data_x;
  ExprStorage data_y;
  std::vector<std::string> data_colors;
  std::vector<std::string> data_sizes;
  ColorMap color_map;
  MeasureMap size_map;

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
    {"shape", std::bind(&marker_configure, _1, &c->shape)},
    {"shapes", std::bind(&marker_configure_list, _1, &c->shapes)},
    {"size", std::bind(&expr_to_size, _1, layer, &c->size)},
    {"sizes", std::bind(&data_load_strings, _1, &data_sizes)},
    {"size-map", std::bind(&measure_map_read, ctx, _1, &size_map)},
    {"color", std::bind(&color_read, ctx, _1, &c->color)},
    {"colors", std::bind(&data_load_strings, _1, &data_colors)},
    {"color-map", std::bind(&color_map_read, ctx, _1, &color_map)},
    {"labels", std::bind(&data_load_strings, _1, &c->labels)},
    {"label-font", expr_call_string_fn(std::bind(&font_load_best, _1, &c->label_font))},
    {"label-font-size", std::bind(&expr_to_font_size, _1, layer, &c->label_font_size)},
    {"label-color", std::bind(&color_read, ctx, _1, &c->label_color)},
    {"label-padding", std::bind(&expr_to_size, _1, layer, &c->label_padding)},
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

  /* convert color data */
  for (const auto& value : data_colors) {
    Color color;
    if (color_map) {
      if (auto rc = color_map(value, &color); !rc) {
        return rc;
      }
    } else {
      if (auto rc = color.parse(value); !rc) {
        return errorf(
            ERROR,
            "invalid data; can't parse '{}' as a color hex code; maybe you "
            "forgot to set the 'color-map' option?",
            value);
      }
    }

    c->colors.push_back(color);
  }

  /* convert size data */
  for (const auto& value : data_sizes) {
    Measure m;
    if (size_map) {
      if (auto rc = size_map(value, &m); !rc) {
        return rc;
      }
    } else {
      if (auto rc = parse_measure(value, &m); !rc) {
        return rc;
      }
    }

    c->sizes.push_back(m);
  }

  return OK;
}

ReturnCode points_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotPointsConfig conf;

  if (auto rc = points_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  return points_draw(ctx, plot, &conf);
}

ReturnCode points_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotPointsConfig conf;
  if (auto rc = points_configure(ctx, plot, &conf, expr); !rc) {
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


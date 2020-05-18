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
#include "rectangles.h"
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

struct PlotRectanglesConfig {
  DataBuffer x;
  DataBuffer y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  DataBuffer size_x;
  DataBuffer size_y;
  Number size_x_default;
  Number size_y_default;
  Color color;
  std::vector<Color> colors;
};

ReturnCode rectangles_draw(
    Context* ctx,
    PlotConfig* plot,
    PlotRectanglesConfig* config) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));
  const auto& scale_x = config->scale_x;
  const auto& scale_y = config->scale_y;

  /* transform data */
  std::vector<double> xs;
  if (auto rc = scale_translatev(scale_x, config->x, &xs); !rc) {
    return rc;
  }

  std::vector<double> ys;
  if (auto rc = scale_translatev(scale_y, config->y, &ys); !rc) {
    return rc;
  }

  std::vector<double> sizes_x;
  if (auto rc = scale_translate_magnitudev(scale_x, config->size_x, &sizes_x); !rc) {
    return rc;
  }

  std::vector<double> sizes_y;
  if (auto rc = scale_translate_magnitudev(scale_y, config->size_y, &sizes_y); !rc) {
    return rc;
  }

  /* draw rectangles */
  for (size_t i = 0; i < xs.size(); ++i) {
    auto sx = clip.x + clip.w * xs[i];
    auto sy = clip.y + clip.h * ys[i];

    const auto& color = config->colors.empty()
        ? config->color
        : config->colors[i % config->colors.size()];

    double size_x = sizes_x.empty()
        ? config->size_x_default.value
        : sizes_x[i % sizes_x.size()] * clip.w;

    double size_y = sizes_y.empty()
        ? config->size_y_default.value
        : sizes_y[i % sizes_y.size()] * clip.h;

    DrawCommand rect;
    rect.fill_style.color = color;
    rect.antialiasing_mode = AntialiasingMode::DISABLE;
    path_add_rectangle(&rect.path, Point(sx, sy), {size_x, size_y});
    draw_shape(ctx, rect);
  }

  return OK;
}

ReturnCode rectangles_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotRectanglesConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->color = layer_get(ctx)->foreground_color;

  /* parse properties */
  std::vector<std::string> data_colors;
  ColorMap color_map;

  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data", std::bind(&data_load_points2, _1, &c->x, &c->y)},
    {"data-x", std::bind(&data_load_simple, _1, &c->x)},
    {"data-y", std::bind(&data_load_simple, _1, &c->y)},
    {"data-size-x", std::bind(&data_load_simple, _1, &c->size_x)},
    {"data-size-y", std::bind(&data_load_simple, _1, &c->size_y)},
    {
      "data-size",
      expr_calln_fn({
        std::bind(&data_load_simple, _1, &c->size_x),
        std::bind(&data_load_simple, _1, &c->size_y),
      })
    },
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
      "size",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &c->size_x_default),
        std::bind(&expr_to_size, _1, layer, &c->size_y_default),
      })
    },
    {"size-x", std::bind(&expr_to_size, _1, layer, &c->size_x_default)},
    {"size-y", std::bind(&expr_to_size, _1, layer, &c->size_y_default)},
    {"color", std::bind(&color_read, ctx, _1, &c->color)},
    {"colors", std::bind(&data_load_strings, _1, &data_colors)},
    {"color-map", std::bind(&color_map_read, ctx, _1, &color_map)},
  });

  if (!config_rc) {
    return config_rc;
  }

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

  return OK;
}

ReturnCode rectangles_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotRectanglesConfig conf;

  if (auto rc = rectangles_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  return rectangles_draw(ctx, plot, &conf);
}

ReturnCode rectangles_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotRectanglesConfig conf;
  if (auto rc = rectangles_configure(ctx, plot, &conf, expr); !rc) {
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


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
#include "errorbars.h"

#include "data.h"
#include "context.h"
#include "layout.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "scale.h"
#include "color_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

static const double kDefaultStrokeWidthPT = 1;
static const double kDefaultBarWidthPT = 6;

struct ErrorbarsElement {
  DataBuffer x;
  DataBuffer x_low;
  DataBuffer x_high;
  DataBuffer y;
  DataBuffer y_low;
  DataBuffer y_high;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  Number bar_width;
  std::vector<Color> colors;
  Color stroke_color;
  Number stroke_width;
};

ReturnCode errorbars_draw_bar(
    Context* ctx,
    const ErrorbarsElement& config,
    const Point& from,
    const Point& to,
    const Color& color) {
  auto direction = normalize(sub(to, from));
  auto ortho = vec2{direction.y, direction.x * -1};

  StrokeStyle line_style;
  line_style.color = color;
  line_style.line_width = config.stroke_width;

  draw_line(ctx, from, to, line_style);

  draw_line(
      ctx,
      add(from, mul(ortho, config.bar_width.value * -0.5)),
      add(from, mul(ortho, config.bar_width.value * 0.5)),
      line_style);

  draw_line(
      ctx,
      add(to, mul(ortho, config.bar_width.value * -0.5)),
      add(to, mul(ortho, config.bar_width.value * 0.5)),
      line_style);

  return OK;
}

ReturnCode errorbars_draw(
    Context* ctx,
    PlotConfig* plot,
    const ErrorbarsElement& config) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));

  /* transform data */
  std::vector<double> x;
  if (auto rc = scale_translatev(config.scale_x, config.x, &x); !rc) {
    return rc;
  }

  std::vector<double> x_low;
  if (auto rc = scale_translatev(config.scale_x, config.x_low, &x_low); !rc) {
    return rc;
  }

  std::vector<double> x_high;
  if (auto rc = scale_translatev(config.scale_x, config.x_high, &x_high); !rc) {
    return rc;
  }

  std::vector<double> y;
  if (auto rc = scale_translatev(config.scale_y, config.y, &y); !rc) {
    return rc;
  }

  std::vector<double> y_low;
  if (auto rc = scale_translatev(config.scale_y, config.y_low, &y_low); !rc) {
    return rc;
  }

  std::vector<double> y_high;
  if (auto rc = scale_translatev(config.scale_y, config.y_high, &y_high); !rc) {
    return rc;
  }

  /* draw errorbars */
  auto x_len = std::min({x.size(), y_low.size(), y_high.size()});
  auto y_len = std::min({y.size(), x_low.size(), x_high.size()});

  for (size_t i = 0; i < x_len; ++i) {
    auto from = vec2(
        clip.x + x[i] * clip.w,
        clip.y + y_low[i] * clip.h);

    auto to = vec2(
        clip.x + x[i] * clip.w,
        clip.y + y_high[i] * clip.h);

    const auto& color = config.colors.empty()
        ? config.stroke_color
        : config.colors[i % config.colors.size()];

    if (auto rc = errorbars_draw_bar(ctx, config, from, to, color); !rc) {
      return rc;
    }
  }

  for (size_t i = 0; i < y_len; ++i) {
    auto from = vec2(
        clip.x + x_low[i] * clip.w,
        clip.y + y[i] * clip.h);

    auto to = vec2(
        clip.x + x_high[i] * clip.w,
        clip.x + y[i] * clip.h);

    const auto& color = config.colors.empty()
        ? config.stroke_color
        : config.colors[i % config.colors.size()];

    if (auto rc = errorbars_draw_bar(ctx, config, from, to, color); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode errorbars_configure(
    Context* ctx,
    PlotConfig* plot,
    ErrorbarsElement* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->stroke_width = unit_from_pt(kDefaultStrokeWidthPT, layer_get_dpi(ctx));
  c->bar_width = unit_from_pt(kDefaultBarWidthPT, layer_get_dpi(ctx));
  c->stroke_color = layer_get(ctx)->foreground_color;
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;

  /* parse properties */
  std::vector<std::string> data_colors;
  ColorMap color_map;

  auto config_rc = expr_walk_map(expr, {
    {"data-x", std::bind(&data_load_simple, _1, &c->x)},
    {"data-y", std::bind(&data_load_simple, _1, &c->y)},
    {"data-x-low", std::bind(&data_load_simple, _1, &c->x_low)},
    {"data-x-high", std::bind(&data_load_simple, _1, &c->x_high)},
    {"data-y-low", std::bind(&data_load_simple, _1, &c->y_low)},
    {"data-y-high", std::bind(&data_load_simple, _1, &c->y_high)},
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
    {"width", std::bind(&expr_to_size, _1, layer, &c->bar_width)},
    {"color", std::bind(&color_read, ctx, _1, &c->stroke_color)},
    {"colors", std::bind(&data_load_strings, _1, &data_colors)},
    {"color-map", std::bind(&color_map_read, ctx, _1, &color_map)},
    {"stroke-color", std::bind(&color_read, ctx, _1, &c->stroke_color)},
    {"stroke-width", std::bind(&expr_to_size, _1, layer, &c->stroke_width)}
  });

  if (!config_rc) {
    return config_rc;
  }

  /* figure out in which direction the user wants us to plot the error bars */
  Option<Direction> direction;
  if (databuf_len(c->x) > 0 &&
      databuf_len(c->y_low) > 0 &&
      databuf_len(c->y_high) > 0 &&
      databuf_len(c->y) == 0) {
    direction = Direction::VERTICAL;

    if (databuf_len(c->x) != databuf_len(c->y_low) ||
        databuf_len(c->x) != databuf_len(c->y_high)) {
      return error(
          ERROR,
          "the length of the 'data-x', 'data-y-low' and 'data-y-high' datasets "
          "must be equal");
    }
  }

  if (databuf_len(c->y) > 0 &&
      databuf_len(c->x_low) > 0 &&
      databuf_len(c->x_high) > 0 &&
      databuf_len(c->x) == 0) {
    direction = Direction::HORIZONTAL;

    if (databuf_len(c->y) != databuf_len(c->x_low) ||
        databuf_len(c->y) != databuf_len(c->x_high)) {
      return error(
          ERROR,
          "the length of the 'data-x', 'data-y-low' and 'data-y-high' datasets "
          "must be equal");
    }
  }

  if (!direction) {
    return error(
        ERROR,
        "the errorbars element expects exactly three data properties:\n"
        "  - vertical bars: data-x, data-y-low, data-y-high\n"
        "  - horizontal bars: data-y, data-x-low, data-x-high");
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

ReturnCode errorbars_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  ErrorbarsElement conf;

  if (auto rc = errorbars_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  return errorbars_draw(ctx, plot, conf);
}

ReturnCode errorbars_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  ErrorbarsElement conf;
  if (auto rc = errorbars_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_x, conf.x); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_x, conf.x_low); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_x, conf.x_high); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_y, conf.y); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_y, conf.y_low); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_y, conf.y_high); !rc) {
    return rc;
  }

  return OK;
}

} // namespace clip::plotgen


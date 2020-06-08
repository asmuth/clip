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
#include "areas.h"

#include "data.h"
#include "context.h"
#include "layout.h"
#include "scale.h"
#include "style.h"
#include "style_reader.h"
#include "color_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

struct PlotAreaConfig {
  PlotAreaConfig();
  Direction direction;
  DataBuffer x;
  DataBuffer xoffset;
  DataBuffer y;
  DataBuffer yoffset;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  StrokeStyle stroke_high_style;
  StrokeStyle stroke_low_style;
  FillStyle fill_style;
};

PlotAreaConfig::PlotAreaConfig() :
    direction(Direction::VERTICAL) {}

ReturnCode areas_draw_horizontal(
    Context* ctx,
    PlotConfig* plot,
    const PlotAreaConfig& conf) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));

  /* transform data */
  std::vector<double> xs;
  if (auto rc = scale_translatev(conf.scale_x, conf.x, &xs); !rc) {
    return rc;
  }

  std::vector<double> xoffsets;
  if (auto rc = scale_translatev(conf.scale_x, conf.xoffset, &xoffsets); !rc) {
    return rc;
  }

  std::vector<double> ys;
  if (auto rc = scale_translatev(conf.scale_y, conf.y, &ys); !rc) {
    return rc;
  }

  std::vector<double> yoffsets;
  if (auto rc = scale_translatev(conf.scale_y, conf.yoffset, &yoffsets); !rc) {
    return rc;
  }

  /* draw areas */
  DrawCommand shape;
  shape.fill_style = conf.fill_style;

  DrawCommand stroke_high;
  stroke_high.stroke_style = conf.stroke_high_style;

  DrawCommand stroke_low;
  stroke_low.stroke_style = conf.stroke_low_style;

  for (size_t i = 0; i < xs.size(); ++i) {
    auto sx = clip.x + xs[i] * clip.w;
    auto sy = clip.y + ys[i] * clip.h;

    if (i == 0) {
      shape.path.moveTo(sx, sy);
      stroke_high.path.moveTo(sx, sy);
    } else {
      shape.path.lineTo(sx, sy);
      stroke_high.path.lineTo(sx, sy);
    }
  }

  auto x0 = std::clamp(scale_translate(conf.scale_x, 0), 0.0, 1.0);
  for (int i = xs.size() - 1; i >= 0; --i) {
    auto sx = clip.x + (xoffsets.empty() ? x0 : xoffsets[i]) * clip.w;
    auto sy = clip.y + ys[i] * clip.h;
    shape.path.lineTo(sx, sy);

    if (stroke_low.path.empty()) {
      stroke_low.path.moveTo(sx, sy);
    } else {
      stroke_low.path.lineTo(sx, sy);
    }
  }

  shape.path.closePath();

  draw_shape(ctx, shape);
  draw_shape(ctx, stroke_high);
  draw_shape(ctx, stroke_low);

  return OK;
}

ReturnCode areas_draw_vertical(
    Context* ctx,
    PlotConfig* plot,
    PlotAreaConfig conf) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));

  /* transform data */
  std::vector<double> xs;
  if (auto rc = scale_translatev(conf.scale_x, conf.x, &xs); !rc) {
    return rc;
  }

  std::vector<double> xoffsets;
  if (auto rc = scale_translatev(conf.scale_x, conf.xoffset, &xoffsets); !rc) {
    return rc;
  }

  std::vector<double> ys;
  if (auto rc = scale_translatev(conf.scale_y, conf.y, &ys); !rc) {
    return rc;
  }

  std::vector<double> yoffsets;
  if (auto rc = scale_translatev(conf.scale_y, conf.yoffset, &yoffsets); !rc) {
    return rc;
  }

  /* draw areas */
  DrawCommand shape;
  shape.fill_style = conf.fill_style;

  DrawCommand stroke_high;
  stroke_high.stroke_style = conf.stroke_high_style;

  DrawCommand stroke_low;
  stroke_low.stroke_style = conf.stroke_low_style;

  for (size_t i = 0; i < xs.size(); ++i) {
    auto sx = clip.x + xs[i] * clip.w;
    auto sy = clip.y + ys[i] * clip.h;

    if (i == 0) {
      shape.path.moveTo(sx, sy);
      stroke_high.path.moveTo(sx, sy);
    } else {
      shape.path.lineTo(sx, sy);
      stroke_high.path.lineTo(sx, sy);
    }
  }

  auto y0 = std::clamp(scale_translate(conf.scale_y, 0), 0.0, 1.0);
  for (int i = xs.size() - 1; i >= 0; --i) {
    auto sx = clip.x + xs[i] * clip.w;
    auto sy = clip.y + (yoffsets.empty() ? y0 : yoffsets[i]) * clip.h;
    shape.path.lineTo(sx, sy);

    if (stroke_low.path.empty()) {
      stroke_low.path.moveTo(sx, sy);
    } else {
      stroke_low.path.lineTo(sx, sy);
    }
  }

  shape.path.closePath();

  draw_shape(ctx, shape);
  draw_shape(ctx, stroke_high);
  draw_shape(ctx, stroke_low);

  return OK;
}

ReturnCode areas_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotAreaConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->stroke_high_style.color = layer_get(ctx)->foreground_color;
  c->stroke_low_style.color = layer_get(ctx)->foreground_color;
  c->fill_style.color = layer_get(ctx)->foreground_color;

  /* parse properties */
  auto config_rc = expr_walk_map(expr, nullptr, {
    {"data", std::bind(&data_load_points2, _1, &c->x, &c->y)},
    {"data-x", std::bind(&data_load_simple, _1, &c->x)},
    {"data-y", std::bind(&data_load_simple, _1, &c->y)},
    {"data-x-high", std::bind(&data_load_simple, _1, &c->x)},
    {"data-y-high", std::bind(&data_load_simple, _1, &c->y)},
    {"data-x-low", std::bind(&data_load_simple, _1, &c->xoffset)},
    {"data-y-low", std::bind(&data_load_simple, _1, &c->yoffset)},
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
      "stroke-color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &c->stroke_high_style.color),
        std::bind(&color_read, ctx, _1, &c->stroke_low_style.color),
      })
    },
    {
      "stroke-width",
      expr_calln_fn({
        std::bind(&expr_to_size, _1, layer, &c->stroke_high_style.line_width),
        std::bind(&expr_to_size, _1, layer, &c->stroke_low_style.line_width),
      })
    },
    {
      "stroke-style",
      expr_calln_fn({
        std::bind(&stroke_style_read, ctx, _1, &c->stroke_high_style),
        std::bind(&stroke_style_read, ctx, _1, &c->stroke_low_style),
      })
    },
    {"stroke-high-color", std::bind(&color_read, ctx, _1, &c->stroke_high_style.color)},
    {"stroke-high-width", std::bind(&expr_to_size, _1, layer, &c->stroke_high_style.line_width)},
    {"stroke-high-style", std::bind(&stroke_style_read, ctx, _1, &c->stroke_high_style)},
    {"stroke-low-color", std::bind(&color_read, ctx, _1, &c->stroke_low_style.color)},
    {"stroke-low-width", std::bind(&expr_to_size, _1, layer, &c->stroke_low_style.line_width)},
    {"stroke-low-style", std::bind(&stroke_style_read, ctx, _1, &c->stroke_low_style)},
    {"fill", std::bind(&fill_style_read, ctx, _1, &c->fill_style)},
    {
      "color",
      expr_calln_fn({
        std::bind(&color_read, ctx, _1, &c->stroke_high_style.color),
        std::bind(&color_read, ctx, _1, &c->stroke_low_style.color),
        std::bind(&fill_style_read_solid, ctx, _1, &c->fill_style),
      })
    },
    {
      "direction",
      expr_to_enum_fn<Direction>(&c->direction, {
        { "horizontal", Direction::HORIZONTAL },
        { "vertical", Direction::VERTICAL },
      })
    },
  });

  if (!config_rc) {
    return config_rc;
  }

  /* check configuraton */
  if (databuf_len(c->x) != databuf_len(c->y)) {
    return error(ERROR, "The length of the 'data-x' and 'data-y' lists must be equal");
  }

  if (databuf_len(c->xoffset) != 0 &&
      databuf_len(c->x) != databuf_len(c->xoffset)) {
    return error(ERROR, "the length of the 'data-x' and 'data-x-low' properties must be equal");
  }

  if (databuf_len(c->yoffset) != 0 &&
      databuf_len(c->y) != databuf_len(c->yoffset)) {
    return error(ERROR, "the length of the 'data-y' and 'data-y-low' properties must be equal");
  }

  return OK;
}

ReturnCode areas_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotAreaConfig conf;

  if (auto rc = areas_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  switch (conf.direction) {
    case Direction::HORIZONTAL:
      return areas_draw_horizontal(ctx, plot, conf);
    case Direction::VERTICAL:
      return areas_draw_vertical(ctx, plot, conf);
    default:
      return ERROR;
  }
}

ReturnCode areas_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotAreaConfig conf;
  if (auto rc = areas_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_x, conf.x); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_x, conf.xoffset); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_y, conf.y); !rc) {
    return rc;
  }

  if (auto rc = scale_fit(&plot->scale_y, conf.yoffset); !rc) {
    return rc;
  }

  return OK;
}

} // namespace clip::plotgen


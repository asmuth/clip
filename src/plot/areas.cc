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

namespace clip::elements::plot::areas {

struct PlotAreaConfig {
  PlotAreaConfig();
  Direction direction;
  std::vector<Measure> x;
  std::vector<Measure> xoffset;
  std::vector<Measure> y;
  std::vector<Measure> yoffset;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  StrokeStyle stroke_high_style;
  StrokeStyle stroke_low_style;
  FillStyle fill_style;
};

PlotAreaConfig::PlotAreaConfig() :
    direction(Direction::VERTICAL) {}

ReturnCode draw_horizontal(
    Context* ctx,
    PlotAreaConfig config) {
  const auto& clip = context_get_clip(ctx);

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config.stroke_high_style.line_width);

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config.stroke_low_style.line_width);

  /* draw areas */
  draw_cmd::Shape shape;
  shape.fill_style = config.fill_style;

  draw_cmd::Shape stroke_high;
  stroke_high.stroke_style = config.stroke_high_style;

  draw_cmd::Shape stroke_low;
  stroke_low.stroke_style = config.stroke_low_style;

  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + config.y[i];

    if (i == 0) {
      shape.path.moveTo(sx, sy);
      stroke_high.path.moveTo(sx, sy);
    } else {
      shape.path.lineTo(sx, sy);
      stroke_high.path.lineTo(sx, sy);
    }
  }

  auto x0 = clip.h * std::clamp(scale_translate(config.scale_x, 0), 0.0, 1.0);
  for (int i = config.x.size() - 1; i >= 0; --i) {
    auto sx = clip.x + (config.xoffset.empty() ? x0 : config.xoffset[i]);
    auto sy = clip.y + config.y[i];
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

ReturnCode draw_vertical(
    Context* ctx,
    PlotAreaConfig config) {
  const auto& clip = context_get_clip(ctx);

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config.stroke_high_style.line_width);

  convert_unit_typographic(
      ctx->dpi,
      ctx->font_size,
      &config.stroke_low_style.line_width);

  /* draw areas */
  draw_cmd::Shape shape;
  shape.fill_style = config.fill_style;

  draw_cmd::Shape stroke_high;
  stroke_high.stroke_style = config.stroke_high_style;

  draw_cmd::Shape stroke_low;
  stroke_low.stroke_style = config.stroke_low_style;

  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + config.y[i];

    if (i == 0) {
      shape.path.moveTo(sx, sy);
      stroke_high.path.moveTo(sx, sy);
    } else {
      shape.path.lineTo(sx, sy);
      stroke_high.path.lineTo(sx, sy);
    }
  }

  auto y0 = clip.h * std::clamp(scale_translate(config.scale_y, 0), 0.0, 1.0);
  for (int i = config.x.size() - 1; i >= 0; --i) {
    auto sx = clip.x + config.x[i];
    auto sy = clip.y + (config.yoffset.empty() ? y0 : config.yoffset[i]);
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

ReturnCode areas_draw(
    Context* ctx,
    std::shared_ptr<PlotAreaConfig> config) {
  switch (config->direction) {
    case Direction::HORIZONTAL:
      return draw_horizontal(ctx, *config);
    case Direction::VERTICAL:
      return draw_vertical(ctx, *config);
    default:
      return ERROR;
  }
}

ReturnCode areas_draw(
    Context* ctx,
    const Expr* expr) {
  /* set defaults from environment */
  auto c = std::make_shared<PlotAreaConfig>();
  c->scale_x = ctx->scale_x;
  c->scale_y = ctx->scale_y;
  c->stroke_high_style.color = ctx->foreground_color;
  c->stroke_low_style.color = ctx->foreground_color;
  c->fill_style.color = ctx->foreground_color;

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_y;
  std::vector<std::string> data_xoffset;
  std::vector<std::string> data_yoffset;

  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data-x", bind(&data_load, _1, &c->x)},
    {"data-y", bind(&data_load, _1, &c->y)},
    {"data-x-high", bind(&data_load, _1, &c->x)},
    {"data-y-high", bind(&data_load, _1, &c->y)},
    {"data-x-low", bind(&data_load, _1, &c->xoffset)},
    {"data-y-low", bind(&data_load, _1, &c->yoffset)},
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
      "stroke-color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &c->stroke_high_style.color),
        bind(&color_read, ctx, _1, &c->stroke_low_style.color),
      })
    },
    {
      "stroke-width",
      expr_calln_fn({
        bind(&measure_read, _1, &c->stroke_high_style.line_width),
        bind(&measure_read, _1, &c->stroke_low_style.line_width),
      })
    },
    {
      "stroke-style",
      expr_calln_fn({
        bind(&stroke_style_read, ctx, _1, &c->stroke_high_style),
        bind(&stroke_style_read, ctx, _1, &c->stroke_low_style),
      })
    },
    {"stroke-high-color", bind(&color_read, ctx, _1, &c->stroke_high_style.color)},
    {"stroke-high-width", bind(&measure_read, _1, &c->stroke_high_style.line_width)},
    {"stroke-high-style", bind(&stroke_style_read, ctx, _1, &c->stroke_high_style)},
    {"stroke-low-color", bind(&color_read, ctx, _1, &c->stroke_low_style.color)},
    {"stroke-low-width", bind(&measure_read, _1, &c->stroke_low_style.line_width)},
    {"stroke-low-style", bind(&stroke_style_read, ctx, _1, &c->stroke_low_style)},
    {"fill", bind(&fill_style_read, ctx, _1, &c->fill_style)},
    {
      "color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &c->stroke_high_style.color),
        bind(&color_read, ctx, _1, &c->stroke_low_style.color),
        bind(&fill_style_read_solid, ctx, _1, &c->fill_style),
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

  /* scale configuration */
  if (auto rc = data_to_measures(data_x, c->scale_x, &c->x); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_xoffset, c->scale_x, &c->xoffset); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_y, c->scale_y, &c->y); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_yoffset, c->scale_y, &c->yoffset); !rc){
    return rc;
  }

  for (const auto& v : c->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->xoffset) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
  }

  for (const auto& v : c->yoffset) {
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

  if (!c->xoffset.empty() &&
      c->xoffset.size() != c->x.size()) {
    return error(
        ERROR,
        "the length of the 'data-x' and 'data-x-low' properties must be equal");
  }

  if (!c->yoffset.empty() &&
      c->yoffset.size() != c->y.size()) {
    return error(
        ERROR,
        "the length of the 'data-y' and 'data-y-low' properties must be equal");
  }

  return areas_draw(ctx, c);
}

} // namespace clip::elements::plot::areas


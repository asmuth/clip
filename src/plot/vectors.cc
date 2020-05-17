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
#include "vectors.h"
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "context.h"
#include "color_reader.h"
#include "typographic_map.h"
#include "typographic_reader.h"
#include "layout.h"
#include "arrows.h"
#include "scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::plotgen {

static const double kDefaultArrowSizePT = 1;

struct PlotVectorsConfig {
  std::vector<Measure> x;
  std::vector<Measure> y;
  std::vector<Measure> dx;
  std::vector<Measure> dy;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  Color color;
  std::vector<Color> colors;
  Number size;
  std::vector<Measure> sizes;
  Arrow shape;
  std::vector<Arrow> shapes;
};

ReturnCode vectors_draw(
    Context* ctx,
    PlotConfig* plot,
    PlotVectorsConfig* config) {
  const auto& clip = plot_get_clip(plot, layer_get(ctx));

  /* convert units */
  convert_units(
      {
        std::bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        std::bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x.begin(),
      &*config->x.end());

  convert_units(
      {
        std::bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        std::bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  convert_units(
      {
        std::bind(&convert_unit_user, scale_translate_magnitude_fn(config->scale_x), _1),
        std::bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->dx.begin(),
      &*config->dx.end());

  convert_units(
      {
        std::bind(&convert_unit_user, scale_translate_magnitude_fn(config->scale_y), _1),
        std::bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->dy.begin(),
      &*config->dy.end());

  convert_units(
      {
      },
      &*config->sizes.begin(),
      &*config->sizes.end());

  /* draw vectors */
  for (size_t i = 0; i < config->x.size(); ++i) {
    auto sx = clip.x + config->x[i];
    auto sy = clip.y + config->y[i];
    auto dx = config->dx[i];
    auto dy = config->dy[i];

    const auto& color = config->colors.empty()
        ? config->color
        : config->colors[i % config->colors.size()];

    auto size = config->sizes.empty()
        ? config->size
        : Number(config->sizes[i % config->sizes.size()]);

    auto shape = config->shapes.empty()
        ? config->shape
        : config->shapes[i % config->shapes.size()];

    if (auto rc = shape(ctx, {sx, sy}, {sx + dx, sy + dy}, size, color); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode vectors_configure(
    Context* ctx,
    PlotConfig* plot,
    PlotVectorsConfig* c,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  /* set defaults from environment */
  c->scale_x = plot->scale_x;
  c->scale_y = plot->scale_y;
  c->color = layer_get(ctx)->foreground_color;
  c->size = unit_from_pt(kDefaultArrowSizePT, layer_get_dpi(ctx));
  c->shape = arrow_create_default();

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_y;
  std::vector<std::string> data_dx;
  std::vector<std::string> data_dy;
  std::vector<std::string> data_colors;
  std::vector<std::string> data_sizes;
  ColorMap color_map;
  MeasureMap size_map;

  auto config_rc = expr_walk_map_wrapped(expr, {
    {"data-x", std::bind(&data_load_strings, _1, &data_x)},
    {"data-y", std::bind(&data_load_strings, _1, &data_y)},
    {"data-dx", std::bind(&data_load_strings, _1, &data_dx)},
    {"data-dy", std::bind(&data_load_strings, _1, &data_dy)},
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
    {"color", std::bind(&color_read, ctx, _1, &c->color)},
    {"colors", std::bind(&data_load_strings, _1, &data_colors)},
    {"color-map", std::bind(&color_map_read, ctx, _1, &color_map)},
    {"size", std::bind(&expr_to_size, _1, layer, &c->size)},
    {"sizes", std::bind(&data_load_strings, _1, &data_sizes)},
    {"size-map", std::bind(&measure_map_read, ctx, _1, &size_map)},
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

  if (auto rc = data_to_measures(data_dx, c->scale_x, &c->dx); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_dy, c->scale_y, &c->dy); !rc){
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

  /* check configuration */
  if (c->x.size() != c->y.size() ||
      c->x.size() != c->dx.size() ||
      c->x.size() != c->dy.size()) {
    return error(
        ERROR,
        "the length of the 'data-x' and 'data-y' properties must be equal");
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

ReturnCode vectors_draw(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotVectorsConfig conf;

  if (auto rc = vectors_configure(ctx, plot, &conf, expr); !rc) {
    return rc;
  }

  return vectors_draw(ctx, plot, &conf);
}

ReturnCode vectors_autorange(
    Context* ctx,
    PlotConfig* plot,
    const Expr* expr) {
  PlotVectorsConfig conf;
  return vectors_configure(ctx, plot, &conf, expr);
}

} // namespace clip::plotgen


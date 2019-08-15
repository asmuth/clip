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
#include "errorbars.h"

#include "data.h"
#include "environment.h"
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

namespace fviz::elements::plot::errorbars {

static const double kDefaultStrokeWidthPT = 1;
static const double kDefaultBarWidthPT = 6;

struct ErrorbarsElement {
  std::vector<Measure> x;
  std::vector<Measure> x_low;
  std::vector<Measure> x_high;
  std::vector<Measure> y;
  std::vector<Measure> y_low;
  std::vector<Measure> y_high;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  Measure bar_width;
  std::vector<Color> colors;
  Color stroke_color;
  Measure stroke_width;
};

ReturnCode draw_errorbar(
    const ErrorbarsElement& config,
    const Point& from,
    const Point& to,
    const Color& color,
    Page* layer) {
  auto direction = vec2_normalize(vec2_sub(to, from));
  auto ortho = vec2{direction.y, direction.x * -1};

  StrokeStyle line_style;
  line_style.color = color;
  line_style.line_width = config.stroke_width;

  strokeLine(layer, from, to, line_style);

  strokeLine(layer,
      vec2_add(from, vec2_mul(ortho, config.bar_width * -0.5)),
      vec2_add(from, vec2_mul(ortho, config.bar_width * 0.5)),
      line_style);

  strokeLine(layer,
      vec2_add(to, vec2_mul(ortho, config.bar_width * -0.5)),
      vec2_add(to, vec2_mul(ortho, config.bar_width * 0.5)),
      line_style);

  return OK;
}

ReturnCode draw(
    std::shared_ptr<ErrorbarsElement> config,
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
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x_low.begin(),
      &*config->x_low.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config->x_high.begin(),
      &*config->x_high.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y.begin(),
      &*config->y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y_low.begin(),
      &*config->y_low.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size, _1),
        bind(&convert_unit_user, scale_translate_fn(config->scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config->y_high.begin(),
      &*config->y_high.end());

  convert_unit_typographic(
      layer->dpi,
      layer->font_size,
      &config->stroke_width);

  convert_unit_typographic(
      layer->dpi,
      layer->font_size,
      &config->bar_width);

  auto x_len = std::min({
    config->x.size(),
    config->y_low.size(),
    config->y_high.size(),
  });

  auto y_len = std::min({
    config->y.size(),
    config->x_low.size(),
    config->x_high.size(),
  });

  for (size_t i = 0; i < x_len; ++i) {
    auto from = vec2(
        clip.x + config->x[i],
        clip.y + clip.h - config->y_low[i]);

    auto to = vec2(
        clip.x + config->x[i],
        clip.y + clip.h - config->y_high[i]);

    const auto& color = config->colors.empty()
        ? config->stroke_color
        : config->colors[i % config->colors.size()];

    if (auto rc = draw_errorbar(*config, from, to, color, layer); !rc) {
      return rc;
    }
  }

  for (size_t i = 0; i < y_len; ++i) {
    auto from = vec2(
        clip.x + config->x_low[i],
        clip.y + clip.h - config->y[i]);

    auto to = vec2(
        clip.x + config->x_high[i],
        clip.x + clip.h - config->y[i]);

    const auto& color = config->colors.empty()
        ? config->stroke_color
        : config->colors[i % config->colors.size()];

    if (auto rc = draw_errorbar(*config, from, to, color, layer); !rc) {
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
  auto c = std::make_shared<ErrorbarsElement>();
  c->stroke_width = from_pt(kDefaultStrokeWidthPT);
  c->bar_width = from_pt(kDefaultBarWidthPT);
  c->stroke_color = env.foreground_color;

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_x_low;
  std::vector<std::string> data_x_high;
  std::vector<std::string> data_y;
  std::vector<std::string> data_y_low;
  std::vector<std::string> data_y_high;
  std::vector<std::string> data_colors;
  ColorMap color_map;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"data-x", bind(&data_load_strings, _1, &data_x)},
    {"data-x-low", bind(&data_load_strings, _1, &data_x_low)},
    {"data-x-high", bind(&data_load_strings, _1, &data_x_high)},
    {"data-y", bind(&data_load_strings, _1, &data_y)},
    {"data-y-low", bind(&data_load_strings, _1, &data_y_low)},
    {"data-y-high", bind(&data_load_strings, _1, &data_y_high)},
    {"data-color", bind(&data_load_strings, _1, &data_colors)},
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
    {"errorbar-width", bind(&measure_read, _1, &c->bar_width)},
    {"color", bind(&color_read, env, _1, &c->stroke_color)},
    {"color-map", bind(&color_map_read, env, _1, &color_map)},
    {"stroke-color", bind(&color_read, env, _1, &c->stroke_color)},
    {"stroke-width", bind(&measure_read, _1, &c->stroke_width)}
  });

  if (!config_rc) {
    return config_rc;
  }

  /* figure out in which direction the user wants us to plot the error bars */
  std::optional<Direction> direction;
  if (!data_x.empty() &&
      !data_y_low.empty() &&
      !data_y_high.empty() &&
      data_y.empty()) {
    direction = Direction::VERTICAL;

    if (data_x.size() != data_y_low.size() ||
        data_x.size() != data_y_high.size()) {
      return error(
          ERROR,
          "the length of the 'data-x', 'data-y-low' and 'data-y-high' datasets "
          "must be equal");
    }
  }

  if (!data_y.empty() &&
      !data_x_low.empty() &&
      !data_x_high.empty() &&
      data_x.empty()) {
    direction = Direction::HORIZONTAL;

    if (data_y.size() != data_x_low.size() ||
        data_y.size() != data_x_high.size()) {
      return error(
          ERROR,
          "the length of the 'data-x', 'data-y-low' and 'data-y-high' datasets "
          "must be equal");
    }
  }

  if (!direction) {
    return error(
        ERROR,
        "the errorbars expectes exactly three data properties:\n"
        "  - vertical bars: data-x, data-y-low, data-y-high\n"
        "  - horizontal bars: data-y, data-x-low, data-x-high");
  }

  /* scale autoconfiguration */
  if (auto rc = data_to_measures(data_x, c->scale_x, &c->x); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_x_low, c->scale_x, &c->x_low); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_x_high, c->scale_x, &c->x_high); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_y, c->scale_y, &c->y); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_y_low, c->scale_y, &c->y_low); !rc){
    return rc;
  }

  if (auto rc = data_to_measures(data_y_high, c->scale_y, &c->y_high); !rc){
    return rc;
  }

  for (const auto& v : c->x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->x_low) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->x_high) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_x);
    }
  }

  for (const auto& v : c->y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
  }

  for (const auto& v : c->y_low) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
  }

  for (const auto& v : c->y_high) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &c->scale_y);
    }
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

  /* return element */
  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, c, _1, _2);
  return OK;
}

} // namespace fviz::elements::plot::errorbars


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
#include "bars.h"

#include "data.h"
#include "context.h"
#include "layout.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "scale.h"
#include "style.h"
#include "style_reader.h"
#include "color_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::elements::plot::bars {

static const double kDefaultBarSizePT = 10;
static const double kDefaultLabelPaddingHorizEM = 0.6;
static const double kDefaultLabelPaddingVertEM = 0.6;

struct PlotBarsConfig {
  PlotBarsConfig();
  Direction direction;
  std::vector<Measure> x;
  std::vector<Measure> xoffset;
  std::vector<Measure> y;
  std::vector<Measure> yoffset;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  StrokeStyle stroke_style;
  FillStyle fill_style;
  std::vector<Measure> sizes;
  std::vector<Measure> offsets;
  std::vector<std::string> labels;
  FontInfo label_font;
  Measure label_padding;
  Measure label_font_size;
  Color label_color;
};

PlotBarsConfig::PlotBarsConfig() :
    direction(Direction::VERTICAL) {}

ReturnCode draw_horizontal(
    Context* ctx,
    PlotBarsConfig config) {
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

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1)
      },
      &*config.offsets.begin(),
      &*config.offsets.end());

  /* draw bars */
  auto x0 = std::clamp(clip.h * scale_translate(config.scale_x, 0), 0.0, 1.0);
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sy = clip.y + config.y[i];
    auto sx1 = clip.x + (config.xoffset.empty() ? x0 : config.xoffset[i]);
    auto sx2 = clip.x + config.x[i];

    auto size = config.sizes.empty()
        ? from_pt(kDefaultBarSizePT, ctx->dpi)
        : config.sizes[i % config.sizes.size()];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    Path path;
    path.moveTo(sx1, sy + -offset - size * 0.5);
    path.lineTo(sx2, sy + -offset - size * 0.5);
    path.lineTo(sx2, sy + -offset + size * 0.5);
    path.lineTo(sx1, sy + -offset + size * 0.5);
    path.closePath();

    draw_path(ctx, path, config.stroke_style, config.fill_style);
  }

  /* draw labels */
  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& text = config.labels[i];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    auto padding = measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingHorizEM, config.label_font_size));

    Point p(
        clip.x + config.x[i] + padding,
        clip.y + -offset + config.y[i]);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::LEFT;
    auto ay = VAlign::CENTER;
    if (auto rc = draw_text(ctx, text, p, ax, ay, style); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw_vertical(
    Context* ctx,
    PlotBarsConfig config) {
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

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  convert_units(
      {
        bind(&convert_unit_typographic, ctx->dpi, ctx->font_size, _1)
      },
      &*config.offsets.begin(),
      &*config.offsets.end());

  convert_unit_typographic(ctx->dpi, ctx->font_size, &config.stroke_style.line_width);

  /* draw bars */
  auto y0 = clip.h * std::clamp(scale_translate(config.scale_y, 0), 0.0, 1.0);
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy1 = clip.y + (config.yoffset.empty() ? y0 : config.yoffset[i]);
    auto sy2 = clip.y + config.y[i];

    auto size = config.sizes.empty()
        ? from_pt(kDefaultBarSizePT, ctx->dpi)
        : config.sizes[i % config.sizes.size()];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    Path path;
    path.moveTo(sx + offset - size * 0.5, sy1);
    path.lineTo(sx + offset - size * 0.5, sy2);
    path.lineTo(sx + offset + size * 0.5, sy2);
    path.lineTo(sx + offset + size * 0.5, sy1);
    path.closePath();

    draw_path(ctx, path, config.stroke_style, config.fill_style);
  }

  /* draw labels */
  for (size_t i = 0; i < config.labels.size(); ++i) {
    const auto& text = config.labels[i];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    auto padding = measure_or(
        config.label_padding,
        from_em(kDefaultLabelPaddingVertEM, config.label_font_size));

    Point p(
        clip.x + offset + config.x[i],
        clip.y + config.y[i] + padding);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = draw_text(ctx, text, p, ax, ay, style); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw(
    Context* ctx,
    std::shared_ptr<PlotBarsConfig> config) {
  switch (config->direction) {
    case Direction::HORIZONTAL:
      return draw_horizontal(ctx, *config);
    case Direction::VERTICAL:
      return draw_vertical(ctx, *config);
    default:
      return ERROR;
  }
}

ReturnCode bars_draw(
    Context* ctx,
    const Expr* expr) {
  /* set defaults from environment */
  auto c = std::make_shared<PlotBarsConfig>();
  c->stroke_style.color = ctx->foreground_color;
  c->stroke_style.line_width = from_unit(0);
  c->fill_style.color = ctx->foreground_color;
  c->label_font = ctx->font;
  c->label_font_size = ctx->font_size;

  /* parse properties */
  std::vector<std::string> data_x;
  std::vector<std::string> data_y;
  std::vector<std::string> data_xoffset;
  std::vector<std::string> data_yoffset;

  auto config_rc = expr_walk_map_with_defaults(expr_next(expr), ctx->defaults, {
    {"data-x", bind(&data_load_strings, _1, &data_x)},
    {"data-y", bind(&data_load_strings, _1, &data_y)},
    {"data-x-high", bind(&data_load_strings, _1, &data_x)},
    {"data-y-high", bind(&data_load_strings, _1, &data_y)},
    {"data-x-low", bind(&data_load_strings, _1, &data_xoffset)},
    {"data-y-low", bind(&data_load_strings, _1, &data_yoffset)},
    {"width", bind(&data_load, _1, &c->sizes)},
    {"widths", bind(&data_load, _1, &c->sizes)},
    {"offset", bind(&data_load, _1, &c->offsets)},
    {"offsets", bind(&data_load, _1, &c->offsets)},
    {"stroke-color", bind(&color_read, ctx, _1, &c->stroke_style.color)},
    {"stroke-width", bind(&measure_read, _1, &c->stroke_style.line_width)},
    {"stroke-style", bind(&stroke_style_read, ctx, _1, &c->stroke_style)},
    {"fill", bind(&fill_style_read, ctx, _1, &c->fill_style)},
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
      "color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &c->stroke_style.color),
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
    {"labels", bind(&data_load_strings, _1, &c->labels)},
    {"label-font-size", bind(&measure_read, _1, &c->label_font_size)},
    {"label-color", bind(&color_read, ctx, _1, &c->label_color)},
    {"label-padding", bind(&measure_read, _1, &c->label_padding)},
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

  return draw(ctx, c);
}

} // namespace clip::elements::plot::bars


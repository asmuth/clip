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
#include "bars.h"

#include "data.h"
#include "environment.h"
#include "layout.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"
#include "scale.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>

using namespace std::placeholders;

namespace fviz::elements::chart::bars {

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
  std::vector<Color> colors;
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
    PlotBarsConfig config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.offsets.begin(),
      &*config.offsets.end());

  /* draw bars */
  auto x0 = std::clamp(clip.h * scale_translate(config.scale_x, 0), 0.0, 1.0);
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sy = clip.y + clip.h - config.y[i];
    auto sx1 = clip.x + (config.xoffset.empty() ? x0 : config.xoffset[i]);
    auto sx2 = clip.x + config.x[i];

    auto size = config.sizes.empty()
        ? from_pt(kDefaultBarSizePT, layer->dpi)
        : config.sizes[i % config.sizes.size()];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    const auto& color = config.colors.empty()
        ? Color{}
        : config.colors[i % config.colors.size()];

    FillStyle style;
    style.color = color;

    Path path;
    path.moveTo(sx1, sy + -offset - size * 0.5);
    path.lineTo(sx2, sy + -offset - size * 0.5);
    path.lineTo(sx2, sy + -offset + size * 0.5);
    path.lineTo(sx1, sy + -offset + size * 0.5);
    path.closePath();


    fillPath(layer, clip, path, style);
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
        clip.y + -offset + clip.h - config.y[i]);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::LEFT;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw_vertical(
    PlotBarsConfig config,
    const LayoutInfo& layout,
    Layer* layer) {
  const auto& clip = layout.content_box;

  /* convert units */
  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.x.begin(),
      &*config.x.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_x), _1),
        bind(&convert_unit_relative, clip.w, _1)
      },
      &*config.xoffset.begin(),
      &*config.xoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.y.begin(),
      &*config.y.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1),
        bind(&convert_unit_user, scale_translate_fn(config.scale_y), _1),
        bind(&convert_unit_relative, clip.h, _1)
      },
      &*config.yoffset.begin(),
      &*config.yoffset.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.sizes.begin(),
      &*config.sizes.end());

  convert_units(
      {
        bind(&convert_unit_typographic, layer->dpi, layer->font_size.value, _1)
      },
      &*config.offsets.begin(),
      &*config.offsets.end());

  /* draw bars */
  auto y0 = clip.h * std::clamp(scale_translate(config.scale_y, 0), 0.0, 1.0);
  for (size_t i = 0; i < config.x.size(); ++i) {
    auto sx = clip.x + config.x[i];
    auto sy1 = clip.y + clip.h - (config.yoffset.empty() ? y0 : config.yoffset[i]);
    auto sy2 = clip.y + clip.h - config.y[i];

    auto size = config.sizes.empty()
        ? from_pt(kDefaultBarSizePT, layer->dpi)
        : config.sizes[i % config.sizes.size()];

    auto offset = config.offsets.empty()
        ? 0
        : config.offsets[i % config.offsets.size()];

    const auto& color = config.colors.empty()
        ? Color{}
        : config.colors[i % config.colors.size()];

    FillStyle style;
    style.color = color;

    Path path;
    path.moveTo(sx + offset - size * 0.5, sy1);
    path.lineTo(sx + offset - size * 0.5, sy2);
    path.lineTo(sx + offset + size * 0.5, sy2);
    path.lineTo(sx + offset + size * 0.5, sy1);
    path.closePath();

    fillPath(layer, clip, path, style);
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
        clip.y + clip.h - config.y[i] - padding);

    TextStyle style;
    style.font = config.label_font;
    style.color = config.label_color;
    style.font_size = config.label_font_size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::BOTTOM;
    if (auto rc = drawTextLabel(text, p, ax, ay, style, layer); rc != OK) {
      return rc;
    }
  }

  return OK;
}

ReturnCode draw(
    std::shared_ptr<PlotBarsConfig> config,
    const LayoutInfo& layout,
    Layer* layer) {
  switch (config->direction) {
    case Direction::HORIZONTAL:
      return draw_horizontal(*config, layout, layer);
    case Direction::VERTICAL:
      return draw_vertical(*config, layout, layer);
    default:
      return ERROR;
  }
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  /* set defaults from environment */
  auto c = std::make_shared<PlotBarsConfig>();
  c->label_font = env.font;
  c->label_font_size = env.font_size;

  /* parse properties */
  auto config_rc = expr_walk_map(expr_next(expr), {
    {"data-x", bind(&data_load, _1, &c->x)},
    {"data-y", bind(&data_load, _1, &c->y)},
    {"data-x-offset", bind(&data_load, _1, &c->xoffset)},
    {"data-y-offset", bind(&data_load, _1, &c->yoffset)},
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
    {"color", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &c->colors)},
    {"colors", expr_tov_fn<Color>(bind(&expr_to_color, _1, _2), &c->colors)},
    {
      "direction",
      expr_to_enum_fn<Direction>(&c->direction, {
        { "horizontal", Direction::HORIZONTAL },
        { "vertical", Direction::VERTICAL },
      })
    },
    {"labels", bind(&data_load_strings, _1, &c->labels)},
  });

  if (!config_rc) {
    return config_rc;
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
        "the length of the 'data-x' and 'data-x-offset' properties must be equal");
  }

  if (!c->yoffset.empty() &&
      c->yoffset.size() != c->y.size()) {
    return error(
        ERROR,
        "the length of the 'data-y' and 'data-y-offset' properties must be equal");
  }


  /* scale autoconfig */
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

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, c, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::bars


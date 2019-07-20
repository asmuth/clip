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
#include "data.h"
#include "layout.h"
#include "element_factory.h"
#include "graphics/layer.h"
#include "scale.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>
#include <functional>

using namespace std::placeholders;

namespace fviz::elements::chart::layout {

struct PlotConfig {
  FontInfo font;
  Measure font_size;
  Color text_color;
  Color border_color;
  std::array<Measure, 4> margins;
  std::optional<Color> background;
  std::array<StrokeStyle, 4> borders;
  std::vector<ElementRef> body_elements;
  std::array<std::vector<ElementRef>, 4> margin_elements;
};

struct PlotGeom {
  std::string elem_name;
  ExprStorage opts;
};

ReturnCode build_geom(
    const std::string& geom_elem_name,
    const Expr* expr,
    std::vector<PlotGeom>* geoms,
    std::vector<Measure>* x,
    std::vector<Measure>* y) {
  auto opts = expr_clone(expr_get_list(expr));

  auto config_rc = expr_walk_map(opts.get(), {
    {"data-x", bind(&data_load, _1, x)},
    {"data-y", bind(&data_load, _1, y)},
    {"data-x-offset", bind(&data_load, _1, x)},
    {"data-y-offset", bind(&data_load, _1, y)},
  }, false);

  if (!config_rc) {
    return config_rc;
  }

  PlotGeom g;
  g.elem_name = geom_elem_name;
  g.opts = std::move(opts);
  geoms->emplace_back(std::move(g));
  return OK;
}

ReturnCode draw(
    std::shared_ptr<PlotConfig> config,
    const LayoutInfo& layout,
    Layer* layer) {
  /* convert units  */
  auto margins = config->margins;
  for (auto& m : margins) {
    convert_unit_typographic(layer->dpi, config->font_size, &m);
  }

  /* calculate the outer margin box */
  auto content_box = layout_margin_box(
      layout.content_box,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  /* calculate the inner body box and the margin boxes */
  std::array<Measure, 4> padding;
  for (size_t i = 0; i < config->margin_elements.size(); ++i) {
    for (const auto& e : config->margin_elements[i]) {
      if (!e->size_hint) {
        continue;
      }

      double e_width = 0;
      double e_height = 0;
      if (auto rc =
            e->size_hint(
              *layer,
              content_box.w,
              content_box.h,
              &e_width,
              &e_height);
          !rc) {
        return rc;
      }

      padding[i] = from_unit(
          std::max(
              double(padding[i]),
              i % 2 ? e_width : e_height));
    }
  }

  auto body_box = layout_margin_box(
      content_box,
      padding[0],
      padding[1],
      padding[2],
      padding[3]);

  std::array<Rectangle, 4> margin_boxes = {
    Rectangle {
      body_box.x,
      content_box.y,
      body_box.w,
      padding[0],
    },
    Rectangle {
      content_box.x + content_box.w - padding[1],
      body_box.y,
      padding[1],
      body_box.h,
    },
    Rectangle {
      body_box.x,
      content_box.y + content_box.h - padding[2],
      body_box.w,
      padding[2],
    },
    Rectangle {
      content_box.x,
      body_box.y,
      padding[3],
      body_box.h,
    }
  };

  /* draw the background */
  if (config->background) {
    const auto& bg_box = body_box;
    FillStyle bg_fill;
    bg_fill.color = *config->background;

    fillRectangle(
        layer,
        Point(bg_box.x, bg_box.y),
        bg_box.w,
        bg_box.h,
        bg_fill);
  }

  /* draw the body elements  */
  for (const auto& e : config->body_elements) {
    LayoutInfo layout;
    layout.content_box = body_box;

    if (auto rc = e->draw(layout, layer); !rc) {
      return rc;
    }
  }

  /* draw the margin elements  */
  for (size_t i = 0; i < config->margin_elements.size(); ++i) {
    for (const auto& e : config->margin_elements[i]) {
      LayoutInfo layout;
      layout.content_box = margin_boxes[i];

      if (auto rc = e->draw(layout, layer); !rc) {
        return rc;
      }
    }
  }

  /* draw the top border  */
  if (config->borders[0].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[0].line_width;
    border_style.color = config->borders[0].color;

    strokeLine(
        layer,
        Point(content_box.x, content_box.y),
        Point(content_box.x + content_box.w, content_box.y),
        border_style);
  }

  /* draw the right border  */
  if (config->borders[1].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[1].line_width;
    border_style.color = config->borders[1].color;

    strokeLine(
        layer,
        Point(content_box.x + content_box.w, content_box.y),
        Point(content_box.x + content_box.w, content_box.y + content_box.h),
        border_style);
  }

  /* draw the bottom border  */
  if (config->borders[2].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[2].line_width;
    border_style.color = config->borders[2].color;

    strokeLine(
        layer,
        Point(content_box.x, content_box.y + content_box.h),
        Point(content_box.x + content_box.w, content_box.y + content_box.h),
        border_style);
  }

  /* draw the left border  */
  if (config->borders[3].line_width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[3].line_width;
    border_style.color = config->borders[3].color;

    strokeLine(
        layer,
        Point(content_box.x, content_box.y),
        Point(content_box.x, content_box.y + content_box.h),
        border_style);
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  auto config = std::make_shared<PlotConfig>();
  config->font = env.font;
  config->font_size = env.font_size;
  config->text_color = env.text_color;
  config->border_color = env.border_color;
  config->margins = {from_em(1), from_em(1), from_em(1), from_em(1)};

  std::vector<Measure> x;
  std::vector<Measure> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::set<std::string> axes;
  std::vector<ExprStorage> layout_opts;
  std::vector<PlotGeom> geoms;
  std::vector<ExprStorage> geom_opts;
  std::vector<ExprStorage> axis_opts;
  std::vector<ExprStorage> axis_x_opts;
  std::vector<ExprStorage> axis_y_opts;
  ExprStorage grid_opts;
  std::vector<ExprStorage> grid_extra_opts;
  ExprStorage legend_opts;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"limit-x", bind(&expr_to_float64_opt_pair, _1, &scale_x.min, &scale_x.max)},
    {"limit-x-min", bind(&expr_to_float64_opt, _1, &scale_x.min)},
    {"limit-x-max", bind(&expr_to_float64_opt, _1, &scale_x.max)},
    {"limit-y", bind(&expr_to_float64_opt_pair, _1, &scale_y.min, &scale_y.max)},
    {"limit-y-min", bind(&expr_to_float64_opt, _1, &scale_y.min)},
    {"limit-y-max", bind(&expr_to_float64_opt, _1, &scale_y.max)},
    {
      "scale-x",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &scale_x),
        bind(&expr_rewritev, _1, "scale-x", &geom_opts),
        bind(&expr_rewritev, _1, "scale", &axis_x_opts)
      })
    },
    {
      "scale-y",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &scale_y),
        bind(&expr_rewritev, _1, "scale-y", &geom_opts),
        bind(&expr_rewritev, _1, "scale", &axis_y_opts)
      })
    },
    {"scale-x-padding", bind(&expr_to_float64, _1, &scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &scale_y.padding)},
    {
      "ticks-x",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "ticks", &axis_x_opts),
        bind(&expr_rewritev, _1, "ticks-x", &grid_extra_opts),
      })
    },
    {
      "ticks-y",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "ticks", &axis_y_opts),
        bind(&expr_rewritev, _1, "ticks-y", &grid_extra_opts),
      })
    },
    {"axis-x-format", bind(&expr_rewritev, _1, "format", &axis_x_opts)},
    {"axis-y-format", bind(&expr_rewritev, _1, "format", &axis_y_opts)},
    {"axes", bind(&expr_to_stringset, _1, &axes)},
    {"areas", bind(&build_geom, "chart/areas", _1, &geoms, &x, &y)},
    {"bars", bind(&build_geom, "chart/bars", _1, &geoms, &x, &y)},
    {"lines", bind(&build_geom, "chart/lines", _1, &geoms, &x, &y)},
    {"labels", bind(&build_geom, "chart/labels", _1, &geoms, &x, &y)},
    {"points", bind(&build_geom, "chart/points", _1, &geoms, &x, &y)},
    {"grid", bind(&expr_to_copy, _1, &grid_opts)},
    {"legend", bind(&expr_to_copy, _1, &legend_opts)},
    {
      "margin",
      expr_calln_fn({
        bind(&expr_to_measure, _1, &config->margins[0]),
        bind(&expr_to_measure, _1, &config->margins[1]),
        bind(&expr_to_measure, _1, &config->margins[2]),
        bind(&expr_to_measure, _1, &config->margins[3]),
      })
    },
    {"margin-top", bind(&expr_to_measure, _1, &config->margins[0])},
    {"margin-right", bind(&expr_to_measure, _1, &config->margins[1])},
    {"margin-bottom", bind(&expr_to_measure, _1, &config->margins[2])},
    {"margin-left", bind(&expr_to_measure, _1, &config->margins[3])},
    {"background", bind(&expr_to_color_opt, _1, &config->background)},
    {
      "border",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "border", &axis_opts),
        bind(&expr_to_stroke_style, _1, &config->borders[0]),
        bind(&expr_to_stroke_style, _1, &config->borders[1]),
        bind(&expr_to_stroke_style, _1, &config->borders[2]),
        bind(&expr_to_stroke_style, _1, &config->borders[3])
      })
    },
    {"border-top", bind(&expr_to_stroke_style, _1, &config->borders[0])},
    {"border-right", bind(&expr_to_stroke_style, _1, &config->borders[1])},
    {"border-bottom", bind(&expr_to_stroke_style, _1, &config->borders[2])},
    {"border-left", bind(&expr_to_stroke_style, _1, &config->borders[3])},
    {
      "border-color",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "border-color", &axis_opts),
        bind(&expr_to_color, _1, &config->border_color)
      })
    },
    {"border-top-color", bind(&expr_to_color, _1, &config->borders[0].color)},
    {"border-right-color", bind(&expr_to_color, _1, &config->borders[1].color)},
    {"border-bottom-color", bind(&expr_to_color, _1, &config->borders[2].color)},
    {"border-left-color", bind(&expr_to_color, _1, &config->borders[3].color)},
    {
      "border-width",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "border-width", &axis_opts),
        bind(&expr_to_measure, _1, &config->borders[0].line_width),
        bind(&expr_to_measure, _1, &config->borders[1].line_width),
        bind(&expr_to_measure, _1, &config->borders[2].line_width),
        bind(&expr_to_measure, _1, &config->borders[3].line_width)
      })
    },
    {"border-top-width", bind(&expr_to_measure, _1, &config->borders[0].line_width)},
    {"border-right-width", bind(&expr_to_measure, _1, &config->borders[1].line_width)},
    {"border-bottom-width", bind(&expr_to_measure, _1, &config->borders[2].line_width)},
    {"border-left-width", bind(&expr_to_measure, _1, &config->borders[3].line_width)},
    {"body", bind(&element_build_list, env, _1, &config->body_elements)},
    {"top", bind(&element_build_list, env, _1, &config->margin_elements[0])},
    {"right", bind(&element_build_list, env, _1, &config->margin_elements[1])},
    {"bottom", bind(&element_build_list, env, _1, &config->margin_elements[2])},
    {"left", bind(&element_build_list, env, _1, &config->margin_elements[3])},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* scale autoconfig */
  for (const auto& v : x) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &scale_x);
    }
  }

  for (const auto& v : y) {
    if (v.unit == Unit::USER) {
      scale_fit(v.value, &scale_y);
    }
  }

  auto xmin = expr_create_value(std::to_string(scale_min(scale_x)));
  auto xmax = expr_create_value(std::to_string(scale_max(scale_x)));
  auto ymin = expr_create_value(std::to_string(scale_min(scale_y)));
  auto ymax = expr_create_value(std::to_string(scale_max(scale_y)));

  /* build the grid */
  if (grid_opts) {
    auto elem_config = expr_build(
        "chart/gridlines",
        "limit-x-min",
        expr_clone(xmin.get()),
        "limit-x-max",
        expr_clone(xmax.get()),
        "limit-y-min",
        expr_clone(ymin.get()),
        "limit-y-max",
        expr_clone(ymax.get()),
        expr_unwrap(std::move(grid_opts)),
        expr_clonev(grid_extra_opts));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->body_elements.emplace_back(elem);
  }

  /* build the top axis */
  if (axes.count("top")) {
    auto elem_config = expr_build(
        "chart/axis-top",
        "limit-min",
        expr_clone(xmin.get()),
        "limit-max",
        expr_clone(xmax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_x_opts));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->margin_elements[0].emplace_back(elem);
  }

  /* build the right axis */
  if (axes.count("right")) {
    auto elem_config = expr_build(
        "chart/axis-right",
        "limit-min",
        expr_clone(ymin.get()),
        "limit-max",
        expr_clone(ymax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_y_opts));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->margin_elements[1].emplace_back(elem);
  }

  /* build the bottom axis */
  if (axes.count("bottom")) {
    auto elem_config = expr_build(
        "chart/axis-bottom",
        "limit-min",
        expr_clone(xmin.get()),
        "limit-max",
        expr_clone(xmax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_x_opts));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->margin_elements[2].emplace_back(elem);
  }

  /* build the left axis */
  if (axes.count("left")) {
    auto elem_config = expr_build(
        "chart/axis-left",
        "limit-min",
        expr_clone(ymin.get()),
        "limit-max",
        expr_clone(ymax.get()),
        expr_clonev(axis_opts),
        expr_clonev(axis_y_opts));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->margin_elements[3].emplace_back(elem);
  }

  /* build the chart (body) geoms */
  geom_opts.emplace_back(expr_create_value("limit-x-min"));
  geom_opts.emplace_back(expr_clone(xmin.get()));
  geom_opts.emplace_back(expr_create_value("limit-x-max"));
  geom_opts.emplace_back(expr_clone(xmax.get()));
  geom_opts.emplace_back(expr_create_value("limit-y-min"));
  geom_opts.emplace_back(expr_clone(ymin.get()));
  geom_opts.emplace_back(expr_create_value("limit-y-max"));
  geom_opts.emplace_back(expr_clone(ymax.get()));

  std::vector<ExprStorage> chart_geoms;
  for (const auto& g : geoms) {
    auto elem_config = expr_build(
        g.elem_name,
        expr_clonev(geom_opts),
        expr_clone(g.opts.get()));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->body_elements.emplace_back(elem);
  }

  /* build the legend */
  if (legend_opts) {
    auto elem_config = expr_build(
        "chart/legend",
        expr_unwrap(std::move(legend_opts)));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->body_elements.emplace_back(elem);
  }

  /* return the element */
  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::chart::layout


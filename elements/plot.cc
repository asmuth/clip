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
#include "plot.h"

#include "data.h"
#include "environment.h"
#include "layout.h"
#include "color_reader.h"
#include "element_factory.h"
#include "graphics/layer.h"
#include "scale.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>
#include <functional>

using namespace std::placeholders;

namespace fviz::elements::plot {

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

struct PlotAxis {
  std::string align;
  ExprStorage opts;
  size_t position;
};

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

ReturnCode configure_geom(
    const std::string& geom_elem_name,
    const Expr* expr,
    std::vector<PlotGeom>* geoms,
    std::vector<std::string>* x,
    std::vector<std::string>* y) {
  auto opts = expr_clone(expr_get_list(expr));

  auto config_rc = expr_walk_map(opts.get(), {
    {"data-x", bind(&data_load_strings, _1, x)},
    {"data-y", bind(&data_load_strings, _1, y)},
    {"data-x-low", bind(&data_load_strings, _1, x)},
    {"data-y-low", bind(&data_load_strings, _1, y)},
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

ReturnCode configure_axis(
    const std::string& align,
    size_t axis_position,
    const Expr* expr,
    std::vector<PlotAxis>* axes) {
  auto opts = expr_clone(expr_get_list(expr));

  PlotAxis a;
  a.align = align;
  a.opts = std::move(opts);
  a.position = axis_position;
  axes->emplace_back(std::move(a));
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
  config->border_color = env.foreground_color;
  config->margins = {from_em(1), from_em(1), from_em(1), from_em(1)};

  std::vector<std::string> x;
  std::vector<std::string> y;
  ScaleConfig scale_x;
  ScaleConfig scale_y;
  std::set<std::string> axes_auto;
  std::vector<ExprStorage> layout_opts;
  std::vector<PlotGeom> geoms;
  std::vector<ExprStorage> geom_opts;
  std::vector<PlotAxis> axes;
  std::vector<ExprStorage> axis_opts;
  std::vector<ExprStorage> axis_x_opts;
  std::vector<ExprStorage> axis_y_opts;
  ExprStorage grid_opts;
  std::vector<ExprStorage> grid_extra_opts;
  ExprStorage legend_overlay;

  auto config_rc = expr_walk_map(expr_next(expr), {
    /* scale options */
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
        bind(&expr_rewritev, _1, "scale-x", &grid_extra_opts),
        bind(&expr_rewritev, _1, "scale", &axis_x_opts)
      })
    },
    {
      "scale-y",
      expr_calln_fn({
        bind(&scale_configure_kind, _1, &scale_y),
        bind(&expr_rewritev, _1, "scale-y", &geom_opts),
        bind(&expr_rewritev, _1, "scale-y", &grid_extra_opts),
        bind(&expr_rewritev, _1, "scale", &axis_y_opts)
      })
    },
    {"scale-x-padding", bind(&expr_to_float64, _1, &scale_x.padding)},
    {"scale-y-padding", bind(&expr_to_float64, _1, &scale_y.padding)},

    /* axis options */
    {"axes", bind(&expr_to_stringset, _1, &axes_auto)},
    {"axis-top", bind(&configure_axis, "bottom", 0, _1, &axes)},
    {"axis-right", bind(&configure_axis, "left", 1, _1, &axes)},
    {"axis-bottom", bind(&configure_axis, "top", 2, _1, &axes)},
    {"axis-left", bind(&configure_axis, "right", 3, _1, &axes)},
    {"axis-x-label-format", bind(&expr_rewritev, _1, "label-format", &axis_x_opts)},
    {"axis-y-label-format", bind(&expr_rewritev, _1, "label-format", &axis_y_opts)},
    {
      "axis-x-label-placement",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "label-placement", &axis_x_opts),
        bind(&expr_rewritev, _1, "tick-placement-x", &grid_extra_opts),
      })
    },
    {
      "axis-y-label-placement",
      expr_calln_fn({
        bind(&expr_rewritev, _1, "label-placement", &axis_y_opts),
        bind(&expr_rewritev, _1, "tick-placement-y", &grid_extra_opts),
      })
    },
    {"axis-x-title", bind(&expr_rewritev, _1, "title", &axis_x_opts)},
    {"axis-y-title", bind(&expr_rewritev, _1, "title", &axis_y_opts)},

    /* geom options */
    {"areas", bind(&configure_geom, "plot/areas", _1, &geoms, &x, &y)},
    {"bars", bind(&configure_geom, "plot/bars", _1, &geoms, &x, &y)},
    {"lines", bind(&configure_geom, "plot/lines", _1, &geoms, &x, &y)},
    {"labels", bind(&configure_geom, "plot/labels", _1, &geoms, &x, &y)},
    {"points", bind(&configure_geom, "plot/points", _1, &geoms, &x, &y)},

    /* grid & legend */
    {"grid", bind(&expr_to_copy, _1, &grid_opts)},
    {"legend-overlay", bind(&expr_to_copy, _1, &legend_overlay)},

    /* extra elements */
    {"body", bind(&element_build_list, env, _1, &config->body_elements)},
    {"top", bind(&element_build_list, env, _1, &config->margin_elements[0])},
    {"right", bind(&element_build_list, env, _1, &config->margin_elements[1])},
    {"bottom", bind(&element_build_list, env, _1, &config->margin_elements[2])},
    {"left", bind(&element_build_list, env, _1, &config->margin_elements[3])},

    /* background, margins, borders */
    {"background", bind(&color_read_opt, env, _1, &config->background)},
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
        bind(&color_read, env, _1, &config->border_color)
      })
    },
    {"border-top-color", bind(&color_read, env, _1, &config->borders[0].color)},
    {"border-right-color", bind(&color_read, env, _1, &config->borders[1].color)},
    {"border-bottom-color", bind(&color_read, env, _1, &config->borders[2].color)},
    {"border-left-color", bind(&color_read, env, _1, &config->borders[3].color)},
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
  });

  if (!config_rc) {
    return config_rc;
  }


  /* configure the scale */
  {
    std::vector<Measure> xs;

    if (auto rc = data_to_measures(x, scale_x, &xs); !rc) {
      return rc;
    }

    for (const auto& v : xs) {
      if (v.unit == Unit::USER) {
        scale_fit(v.value, &scale_x);
      }
    }
  }

  {
    std::vector<Measure> ys;

    if (auto rc = data_to_measures(y, scale_y, &ys); !rc) {
      return rc;
    }

    for (const auto& v : ys) {
      if (v.unit == Unit::USER) {
        scale_fit(v.value, &scale_y);
      }
    }
  }

  auto xmin = expr_create_value(std::to_string(scale_min(scale_x)));
  auto xmax = expr_create_value(std::to_string(scale_max(scale_x)));
  auto ymin = expr_create_value(std::to_string(scale_min(scale_y)));
  auto ymax = expr_create_value(std::to_string(scale_max(scale_y)));


  /* configure the auto axis elements */
  if (axes_auto.empty() && axes.empty()) {
    axes_auto = {"top", "right", "bottom", "left"};
  }

  if (axes_auto.count("top")) {
    PlotAxis a;
    a.align= "bottom";
    a.position = 0;
    axes.emplace_back(std::move(a));
  }

  if (axes_auto.count("right")) {
    PlotAxis a;
    a.align = "left";
    a.position = 1;
    axes.emplace_back(std::move(a));
  }

  if (axes_auto.count("bottom")) {
    PlotAxis a;
    a.align = "top";
    a.position = 2;
    axes.emplace_back(std::move(a));
  }

  if (axes_auto.count("left")) {
    PlotAxis a;
    a.align = "right";
    a.position = 3;
    axes.emplace_back(std::move(a));
  }


  /* build the grid element */
  if (grid_opts) {
    auto elem_config = expr_build(
        "plot/grid",
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


  /* build the axes elements */
  for (const auto& axis : axes) {
    ExprStorage elem_config_defaults;

    switch (axis.position) {
      case 0:
      case 2:
        elem_config_defaults = expr_build(
            "limit-min",
            expr_clone(xmin.get()),
            "limit-max",
            expr_clone(xmax.get()),
            expr_clonev(axis_opts),
            expr_clonev(axis_x_opts));
        break;
      case 1:
      case 3:
        elem_config_defaults = expr_build(
            "limit-min",
            expr_clone(ymin.get()),
            "limit-max",
            expr_clone(ymax.get()),
            expr_clonev(axis_opts),
            expr_clonev(axis_y_opts));
    }

    auto elem_config = expr_build(
        "plot/axis",
        "align",
        axis.align,
        expr_unwrap(std::move(elem_config_defaults)),
        expr_clone(axis.opts.get()));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->margin_elements[axis.position].emplace_back(elem);
  }


  /* build the chart (body) geom elements */
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

  /* build the legend overlay */
  if (legend_overlay) {
    auto elem_config = expr_build(
        "legend",
        expr_unwrap(std::move(legend_overlay)));

    ElementRef elem;
    if (auto rc = element_build_macro(env, elem_config.get(), &elem); !rc) {
      return rc;
    }

    config->body_elements.emplace_back(elem);
  }

  /* return the layout element */
  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::plot


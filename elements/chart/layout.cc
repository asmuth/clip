/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "layout.h"
#include "element_factory.h"
#include "graphics/layer.h"
#include "scale.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <functional>

using namespace std::placeholders;

namespace plotfx::elements::chart::layout {

struct PlotBorderConfig {
  Color color;
  Measure width;
};

struct PlotConfig {
  FontInfo font;
  Measure font_size;
  Color text_color;
  Color border_color;
  std::array<Measure, 4> margins;
  std::optional<Color> background;
  std::array<PlotBorderConfig, 4> borders;
  std::vector<ElementRef> body_elements;
  std::vector<ElementRef> top_elements;
  std::vector<ElementRef> right_elements;
  std::vector<ElementRef> bottom_elements;
  std::vector<ElementRef> left_elements;
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

  /* calculate boxes */
  const auto& content_box = layout.content_box;

  auto body_box = layout_margin_box(
      content_box,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  auto top_box = Rectangle(
      body_box.x,
      content_box.y,
      body_box.w,
      margins[0]);

  auto bottom_box = Rectangle(
      body_box.x,
      content_box.y + content_box.h - margins[2],
      body_box.w,
      margins[2]);

  auto right_box = Rectangle(
      content_box.x + content_box.w - margins[1],
      body_box.y,
      margins[1],
      body_box.h);

  auto left_box = Rectangle(
      content_box.x,
      body_box.y,
      margins[3],
      body_box.h);

  /* draw background */
  if (config->background) {
    const auto& bg_box = layout.content_box;
    FillStyle bg_fill;
    bg_fill.color = *config->background;

    fillRectangle(
        layer,
        Point(bg_box.x, bg_box.y),
        bg_box.w,
        bg_box.h,
        bg_fill);
  }

  /* draw content elements  */
  for (const auto& e : config->body_elements) {
    LayoutInfo layout;
    layout.content_box = body_box;

    if (auto rc = e->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  /* draw top elements  */
  for (const auto& e : config->top_elements) {
    LayoutInfo layout;
    layout.content_box = top_box;

    if (auto rc = e->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  /* draw right elements  */
  for (const auto& e : config->right_elements) {
    LayoutInfo layout;
    layout.content_box = right_box;

    if (auto rc = e->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  /* draw bottom elements  */
  for (const auto& e : config->bottom_elements) {
    LayoutInfo layout;
    layout.content_box = bottom_box;

    if (auto rc = e->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  /* draw left elements  */
  for (const auto& e : config->left_elements) {
    LayoutInfo layout;
    layout.content_box = left_box;

    if (auto rc = e->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  /* draw top border  */
  if (config->borders[0].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[0].width;
    border_style.color = config->borders[0].color;

    strokeLine(
        layer,
        Point(content_box.x, content_box.y),
        Point(content_box.x + content_box.w, content_box.y),
        border_style);
  }

  /* draw right border  */
  if (config->borders[1].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[1].width;
    border_style.color = config->borders[1].color;

    strokeLine(
        layer,
        Point(content_box.x + content_box.w, content_box.y),
        Point(content_box.x + content_box.w, content_box.y + content_box.h),
        border_style);
  }

  /* draw top border  */
  if (config->borders[2].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[2].width;
    border_style.color = config->borders[2].color;

    strokeLine(
        layer,
        Point(content_box.x, content_box.y + content_box.h),
        Point(content_box.x + content_box.w, content_box.y + content_box.h),
        border_style);
  }

  /* draw left border  */
  if (config->borders[3].width > 0) {
    StrokeStyle border_style;
    border_style.line_width = config->borders[3].width;
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
  config->margins = {from_em(2), from_em(2), from_em(2), from_em(2)};

  auto config_rc = expr_walk_map(expr_next(expr), {
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
    {"border-top-color", bind(&expr_to_color, _1, &config->borders[0].color)},
    {"border-right-color", bind(&expr_to_color, _1, &config->borders[1].color)},
    {"border-bottom-color", bind(&expr_to_color, _1, &config->borders[2].color)},
    {"border-left-color", bind(&expr_to_color, _1, &config->borders[3].color)},
    {"border-top-width", bind(&expr_to_measure, _1, &config->borders[0].width)},
    {"border-right-width", bind(&expr_to_measure, _1, &config->borders[1].width)},
    {"border-bottom-width", bind(&expr_to_measure, _1, &config->borders[2].width)},
    {"border-left-width", bind(&expr_to_measure, _1, &config->borders[3].width)},
    {"background-color", bind(&expr_to_color_opt, _1, &config->background)},
    {
      "foreground-color",
      expr_calln_fn({
        bind(&expr_to_color, _1, &config->text_color),
        bind(&expr_to_color, _1, &config->border_color),
      })
    },
    {"text-color", bind(&expr_to_color, _1, &config->text_color)},
    {"border-color", bind(&expr_to_color, _1, &config->border_color)},
    {"body", bind(&element_build_list, env, _1, &config->body_elements)},
    {"top", bind(&element_build_list, env, _1, &config->top_elements)},
    {"right", bind(&element_build_list, env, _1, &config->right_elements)},
    {"bottom", bind(&element_build_list, env, _1, &config->bottom_elements)},
    {"left", bind(&element_build_list, env, _1, &config->left_elements)},
  });

  if (!config_rc) {
    return config_rc;
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace plotfx::elements::chart::layout


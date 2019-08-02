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
#include "eval.h"
#include "graphics/layer_pixmap.h"
#include "graphics/layer_svg.h"
#include "layout.h"
#include "sexpr_parser.h"

namespace fviz {

ReturnCode eval(
    Environment env,
    const Expr* expr,
    const OutputFormat& output_format,
    std::string* output_buffer) {
  if (auto rc = environment_configure(&env, expr); !rc) {
    return rc;
  }

  if (auto rc = environment_setup_defaults(&env); !rc) {
    return rc;
  }

  LayerRef layer;
  ReturnCode layer_rc;
  switch (output_format) {
    case OutputFormat::PNG:
      layer_rc = layer_bind_png(
          env.screen_width,
          env.screen_height,
          env.dpi,
          env.font_size,
          env.background_color,
          [output_buffer] (auto png) {
            *output_buffer = png;
            return OK;
          },
          &layer);
      break;
    case OutputFormat::SVG:
      layer_rc = layer_bind_svg(
          env.screen_width,
          env.screen_height,
          env.dpi,
          env.font_size,
          env.background_color,
          [output_buffer] (auto svg) {
            *output_buffer = svg;
            return OK;
          },
          &layer);
      break;
  }

  if (!layer_rc) {
    return layer_rc;
  }

  layer->font = env.font;
  layer->text_default_script = env.text_default_script;
  layer->text_default_language = env.text_default_language;

  LayoutInfo layout;
  layout.content_box = layout_margin_box(
      Rectangle(0, 0, layer->width, layer->height),
      env.margins[0],
      env.margins[1],
      env.margins[2],
      env.margins[3]);

  std::vector<ElementRef> roots;
  auto rc = try_chain({
    [&] { return element_build_all(env, expr, &roots); },
    [&] () -> ReturnCode {
      for (const auto& e : roots) {
        if (auto rc = e->draw(layout, layer.get()); !rc) {
          return rc;
        }
      }

      return OK;
    },
    [&] { return layer_submit(layer.get()); },
  });

  return rc;
}

ReturnCode eval(
    Environment env,
    const std::string& input,
    const OutputFormat& output_format,
    std::string* output_buffer) {
  ExprStorage expr;
  if (auto rc = expr_parse(input.data(), input.length(), &expr); !rc) {
    return rc;
  }

  return eval(env, expr.get(), output_format, output_buffer);
}

} // namespace fviz


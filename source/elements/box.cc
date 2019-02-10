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
#include "elements/box.h"
#include "element_factory.h"
#include "config_helpers.h"
#include "graphics/layer.h"

#include <functional>

using namespace std::placeholders;

namespace plotfx {
namespace box {

ReturnCode draw(
    const BoxConfig& config,
    const LayoutInfo& layout,
    Layer* layer) {
  /* convert units  */
  auto margins = config.margins;
  for (auto& m : margins) {
    convert_unit_typographic(layer->dpi, config.font_size, &m);
  }

  /* calculate margin box */
  auto margin_box = layout_margin_box(
      layout.content_box,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  /* draw background */
  if (config.background) {
    const auto& bg_box = layout.content_box;
    FillStyle bg_fill;
    bg_fill.color = *config.background;

    fillRectangle(
        layer,
        Point(bg_box.x, bg_box.y),
        bg_box.w,
        bg_box.h,
        bg_fill);
  }

  /* layout and draw children */
  std::vector<ElementPlacement> children;
  for (const auto& c : config.children) {
    ElementPlacement e;
    e.element = c;
    children.emplace_back(e);
  }

  if (auto rc = layout_elements(*layer, margin_box, &children); !rc) {
    return rc;
  }

  for (const auto& c : children) {
    if (auto rc = c.element->draw(c.layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  return OK;
}

ReturnCode reflow(
    const BoxConfig& config,
    const Layer& layer,
    const std::optional<double> max_width,
    const std::optional<double> max_height,
    double* min_width,
    double* min_height) {
  return OK; // TODO
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    BoxConfig* config) {
  config->font = env.font;
  config->font_size = env.font_size;
  config->color_scheme = env.color_scheme;
  config->text_color = env.text_color;
  config->border_color = env.border_color;
  config->scale_x = env.scale_x;
  config->scale_y = env.scale_y;
  config->scale_layout_x = env.scale_layout_x;
  config->scale_layout_y = env.scale_layout_y;

  ParserDefinitions pdefs = {
    {"position", bind(&configure_position, _1, &config->layout.position)},
    {"width", bind(&configure_measure_opt, _1, &config->layout.width)},
    {"height", bind(&configure_measure_opt, _1, &config->layout.height)},
    {
      "margin",
      configure_multiprop({
        bind(&configure_measure, _1, &config->margins[0]),
        bind(&configure_measure, _1, &config->margins[1]),
        bind(&configure_measure, _1, &config->margins[2]),
        bind(&configure_measure, _1, &config->margins[3]),
      })
    },
    {"margin-top", bind(&configure_measure, _1, &config->margins[0])},
    {"margin-right", bind(&configure_measure, _1, &config->margins[1])},
    {"margin-bottom", bind(&configure_measure, _1, &config->margins[2])},
    {"margin-left", bind(&configure_measure, _1, &config->margins[3])},
    {"scale-x", bind(&domain_configure, _1, &config->scale_x)},
    {"scale-x-min", bind(&configure_float_opt, _1, &config->scale_x.min)},
    {"scale-x-max", bind(&configure_float_opt, _1, &config->scale_x.max)},
    {"scale-x-padding", bind(&configure_float, _1, &config->scale_x.padding)},
    {"scale-x-layout", bind(&configure_scale_layout, _1, &config->scale_layout_x)},
    {"scale-y", bind(&domain_configure, _1, &config->scale_y)},
    {"scale-y-min", bind(&configure_float_opt, _1, &config->scale_y.min)},
    {"scale-y-max", bind(&configure_float_opt, _1, &config->scale_y.max)},
    {"scale-y-padding", bind(&configure_float, _1, &config->scale_y.padding)},
    {"scale-y-layout", bind(&configure_scale_layout, _1, &config->scale_layout_y)},
    {"background-color", configure_color_opt(&config->background)},
    {
      "foreground-color",
      configure_multiprop({
          bind(&configure_color, _1, &config->text_color),
          bind(&configure_color, _1, &config->border_color),
      })
    },
    {"text-color", bind(&configure_color, _1, &config->text_color)},
    {"border-color", bind(&configure_color, _1, &config->border_color)},
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

  Environment child_env;
  child_env.screen_width = env.screen_width;
  child_env.screen_height = env.screen_height;
  child_env.dpi = env.dpi;
  child_env.font = config->font;
  child_env.font_size = config->font_size;
  child_env.color_scheme = config->color_scheme;
  child_env.text_color = config->text_color;
  child_env.border_color = config->border_color;
  child_env.background_color = config->background.value_or(env.background_color);
  child_env.scale_x = config->scale_x;
  child_env.scale_y = config->scale_y;
  child_env.scale_layout_x = config->scale_layout_x;
  child_env.scale_layout_y = config->scale_layout_y;

  for (size_t i = 0; i < plist.size(); ++i) {
    if (!plist::is_map(plist[i])) {
      continue;
    }

    const auto& elem_name = plist[i].name;
    const auto& elem_config = plist[i].next.get();

    ElementRef elem;
    auto rc = buildElement(
        elem_name,
        *elem_config,
        child_env,
        &elem);

    if (!rc) {
      return rc;
    }

    config->children.emplace_back(std::move(elem));
  }

  return OK;
}

} // namespace box
} // namespace plotfx


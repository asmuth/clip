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
    const LayoutInfo& parent_layout,
    Layer* layer) {
  auto margins = config.margins;
  for (auto& m : margins) {
    convert_unit_typographic(layer->dpi, config.font_size, &m);
  }

  auto bounding_box = layout_margin_box(
      parent_layout.content_box,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  auto content_box = bounding_box;
  std::vector<Rectangle> element_boxes;
  for (const auto& e : config.children) {
    LayoutInfo layout;
    layout.bounding_box = bounding_box;
    layout.constraint = {true, true};
    layout.content_box = content_box;

    if (auto rc = e->layout(*layer, &layout); !rc.isSuccess()) {
      return rc;
    }

    content_box = layout.content_box;
    element_boxes.emplace_back(layout.element_box);
  }

  for (size_t i = 0; i < config.children.size(); ++i) {
    const auto& element = config.children[i];
    LayoutInfo layout;
    layout.bounding_box = bounding_box;
    layout.constraint = {true, true};
    layout.content_box = content_box;
    layout.element_box = element_boxes[i];

    if (auto rc = element->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  return OK;
}

ReturnCode layout(
    const BoxConfig& config,
    const Layer& layer,
    LayoutInfo* layout) {
  /* nothing to do */
  return OK;
}

ReturnCode configure(
    const plist::PropertyList& plist,
    const Environment& env,
    BoxConfig* config) {
  config->font_size = env.font_size;

  static const ParserDefinitions pdefs = {
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
  };

  if (auto rc = parseAll(plist, pdefs); !rc) {
    return rc;
  }

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
        env,
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


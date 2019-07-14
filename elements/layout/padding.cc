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
#include "config_helpers.h"
#include "environment.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "graphics/color.h"

using namespace std::placeholders;

namespace fviz::elements::layout::padding {

struct PaddingElement {
  Measure font_size;
  std::array<Measure, 4> padding;
  std::vector<ElementRef> elements;
};

ReturnCode draw(
    std::shared_ptr<PaddingElement> config,
    const LayoutInfo& layout,
    Layer* layer) {
  /* convert units  */
  auto padding = config->padding;
  for (auto& m : padding) {
    convert_unit_typographic(layer->dpi, config->font_size, &m);
  }

  auto body_box = layout_margin_box(
      layout.content_box,
      padding[0],
      padding[1],
      padding[2],
      padding[3]);

  for (const auto& e : config->elements) {
    LayoutInfo layout;
    layout.content_box = body_box;

    if (auto rc = e->draw(layout, layer); !rc.isSuccess()) {
      return rc;
    }
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  auto config = std::make_shared<PaddingElement>();
  config->font_size = env.font_size;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {
      "padding",
      expr_calln_fn({
        bind(&expr_to_measure, _1, &config->padding[0]),
        bind(&expr_to_measure, _1, &config->padding[1]),
        bind(&expr_to_measure, _1, &config->padding[2]),
        bind(&expr_to_measure, _1, &config->padding[3]),
      })
    },
    {"padding-top", bind(&expr_to_measure, _1, &config->padding[0])},
    {"padding-right", bind(&expr_to_measure, _1, &config->padding[1])},
    {"padding-bottom", bind(&expr_to_measure, _1, &config->padding[2])},
    {"padding-left", bind(&expr_to_measure, _1, &config->padding[3])},
    {"body", bind(&element_build_list, env, _1, &config->elements)},
  });

  if (!config_rc) {
    return config_rc;
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::layout::padding


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

namespace fviz::elements::fill {

struct FillElement {
  Color color;
};

ReturnCode draw(
    std::shared_ptr<FillElement> config,
    const LayoutInfo& layout,
    Layer* layer) {
  FillStyle style;
  style.color = config->color;

  fillRectangle(
      layer,
      Point(layout.content_box.x, layout.content_box.y),
      layout.content_box.w,
      layout.content_box.h,
      style);

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  auto config = std::make_shared<FillElement>();

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"color", bind(&expr_to_color, _1, &config->color)},
  });

  if (!config_rc) {
    return config_rc;
  }

  *elem = std::make_shared<Element>();
  (*elem)->draw = bind(&draw, config, _1, _2);
  return OK;
}

} // namespace fviz::elements::fill


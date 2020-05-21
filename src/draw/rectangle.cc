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
#include "rectangle.h"
#include "draw/path.h"
#include "style_reader.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::draw {

ReturnCode rectangle(
    Context* ctx,
    const Expr* expr) {
  const auto layer = layer_get(ctx);

  /* read arguments */
  draw_style::compound style;

  vec2 position = {
    layer_get_width(*layer).value * .5,
    layer_get_height(*layer).value * .5
  };

  vec2 size = {
    layer_get_width(*layer).value,
    layer_get_height(*layer).value
  };

  auto config_rc = expr_walk_map(expr, {
    {
      "position",
      std::bind(
          &expr_to_vec2,
          _1,
          layer_get_uconv_width(*layer),
          layer_get_uconv_height(*layer),
          &position)
    },
    {
      "size",
      std::bind(
          &expr_to_vec2,
          _1,
          layer_get_uconv_size(*layer),
          layer_get_uconv_size(*layer),
          &size)
    },
    {"fill", std::bind(&style_read_fill, _1, *layer, &style)},
    {"stroke", std::bind(&style_read_stroke, _1, *layer, &style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  /* draw the rectangle */
  draw::path_op op;
  op.style = style;
  path_add_rectangle(&op.path, position, size);

  return draw::path(op, layer);
}

} // namespace clip::draw


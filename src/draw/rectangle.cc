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
#include "data.h"
#include "sexpr.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "context.h"
#include "color_reader.h"
#include "style_reader.h"
#include "typographic_map.h"
#include "typographic_reader.h"
#include "layout.h"
#include "marker.h"
#include "scale.h"
#include "graphics/path.h"
#include "graphics/brush.h"
#include "graphics/text.h"
#include "graphics/layout.h"

#include <numeric>

using namespace std::placeholders;
using std::bind;

namespace clip::draw {

ReturnCode rectangle(
    Context* ctx,
    const Expr* expr) {
  Rectangle rect;
  FillStyle fill_style;
  StrokeStyle stroke_style;
  stroke_style.line_width = from_pt(1);

  /* read arguments */
  auto config_rc = expr_walk_map(expr, {
    {
      "color",
      expr_calln_fn({
        bind(&color_read, ctx, _1, &stroke_style.color),
        bind(&fill_style_read_solid, ctx, _1, &fill_style),
      })
    },
    {"fill", bind(&fill_style_read, ctx, _1, &fill_style)},
    {"stroke-color", bind(&color_read, ctx, _1, &stroke_style.color)},
    {"stroke-width", bind(&measure_read, _1, &stroke_style.line_width)},
    {"stroke-style", bind(&stroke_style_read, ctx, _1, &stroke_style)},
  });

  if (!config_rc) {
    return config_rc;
  }

  Path p;
  path_add_rectangle(&p, rect);
  draw_path(ctx, p, stroke_style, fill_style);

  return OK;
}

} // namespace clip::draw


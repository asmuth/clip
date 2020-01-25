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
#include "layout.h"
#include "graphics/layout.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "typographic_reader.h"

using namespace std::placeholders;
using std::bind;

namespace clip {

ReturnCode layout_add_margins(Context* ctx, const Expr* expr) {
  std::array<Measure, 4> margins;

  auto config_rc = expr_walk_map_with_defaults(expr_next(expr), ctx->defaults, {
    {
      "margin",
      expr_calln_fn({
        bind(&measure_read, _1, &margins[0]),
        bind(&measure_read, _1, &margins[1]),
        bind(&measure_read, _1, &margins[2]),
        bind(&measure_read, _1, &margins[3]),
      })
    },
    {"margin-top", bind(&measure_read, _1, &margins[0])},
    {"margin-right", bind(&measure_read, _1, &margins[1])},
    {"margin-bottom", bind(&measure_read, _1, &margins[2])},
    {"margin-left", bind(&measure_read, _1, &margins[3])},
  });

  if (!config_rc) {
    return config_rc;
  }

  for (auto& m : margins) {
    convert_unit_typographic(ctx->dpi, ctx->font_size, &m);
  }

  auto bbox = context_get_clip(ctx);
  bbox = layout_margin_box(
      bbox,
      margins[0],
      margins[1],
      margins[2],
      margins[3]);

  ctx->layout_stack.push_back(bbox);
  return OK;
}

} // namespace clip


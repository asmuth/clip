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
#include "draw.h"
#include "draw/text.h"
#include "draw/rectangle.h"
#include "sexpr_util.h"

using namespace std::placeholders;

namespace clip {

ReturnCode draw_eval(
    Context* ctx,
    const Expr* expr) {
  const auto& layer = *layer_get(ctx);

  ExprStorage unparsed;
  return expr_walk_commands(expr, nullptr, {
    {"text", std::bind(&draw::text_eval, ctx, _1)},
    {"rectangle", std::bind(&draw::rectangle, ctx, _1)},
  });

  return OK;
}

} // namespace clip


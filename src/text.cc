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
#include "environment.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "text.h"

using namespace std::placeholders;
using std::bind;

namespace clip::elements::text {

struct TextElement {
  std::string text;
  TextStyle text_style;
};

ReturnCode draw(
    Context* ctx,
    std::shared_ptr<TextElement> config) {
  Point p(50, 50);
  auto ax = HAlign::CENTER;
  auto ay = VAlign::BOTTOM;
  if (auto rc =
        draw_text(
            ctx,
            config->text,
            p,
            ax,
            ay,
            config->text_style);
      rc != OK) {
    return rc;
  }

  return OK;
}

ReturnCode build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  auto config = std::make_shared<TextElement>();
  config->text_style.font = env.font;
  config->text_style.font_size = env.font_size;

  auto config_rc = expr_walk_map(expr_next(expr), {
    {"content", bind(&expr_to_string, _1, &config->text)},
  });

  if (!config_rc) {
    return config_rc;
  }

  return OK;
}

} // namespace clip::elements::text


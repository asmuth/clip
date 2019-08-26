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
#include "eval.h"
#include "graphics/page_export_image.h"
#include "graphics/page_export_svg.h"
#include "layout.h"
#include "sexpr_parser.h"

namespace clip {

ReturnCode eval(
    Environment env,
    const Expr* expr,
    const OutputFormat& output_format,
    std::string* output_buffer) {
  // prepare the environment
  if (auto rc = environment_configure(&env, expr); !rc) {
    return rc;
  }

  if (auto rc = environment_setup_defaults(&env); !rc) {
    return rc;
  }

  // prepare the page
  Page page;
  page.font = env.font;
  page.text_default_script = env.text_default_script;
  page.text_default_language = env.text_default_language;
  page.width = env.screen_width;
  page.height = env.screen_height;
  page.dpi = env.dpi;
  page.font_size = env.font_size;
  page.background_color = env.background_color;

  LayoutInfo layout;
  layout.content_box = layout_margin_box(
      Rectangle(0, 0, page.width, page.height),
      env.margins[0],
      env.margins[1],
      env.margins[2],
      env.margins[3]);

  // build all root elements
  std::vector<ElementRef> roots;
  if (auto rc = element_build_all(env, expr, &roots); !rc) {
    return rc;
  }

  // draw all elements
  for (const auto& e : roots) {
    if (auto rc = e->draw(layout, &page); !rc) {
      return rc;
    }
  }

  // export the page
  ReturnCode export_rc;
  switch (output_format) {
    case OutputFormat::SVG:
      export_rc = page_export_svg(page, output_buffer);
      break;
    case OutputFormat::PNG:
      export_rc = page_export_png(page, output_buffer);
      break;
  }

  return export_rc;
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

} // namespace clip


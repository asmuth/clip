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
#include "graphics/export_image.h"
#include "graphics/export_svg.h"
#include "layout.h"
#include "sexpr_parser.h"
#include "sexpr_conv.h"
#include "sexpr_util.h"
#include "typographic_reader.h"

#include "plot/areas.h"
#include "plot/axis.h"
#include "plot/bars.h"
#include "plot/errorbars.h"
#include "plot/grid.h"
#include "plot/labels.h"
#include "plot/lines.h"
#include "plot/points.h"
#include "plot/polygons.h"
#include "plot/rectangles.h"
#include "plot/vectors.h"
#include "draw.h"
#include "draw/rectangle.h"
#include "figure/legend.h"

using namespace std::placeholders;

namespace clip {

using Module = std::function<ReturnCode (Context*, const Expr*)>;
using ModuleMap = std::unordered_map<std::string, Module>;

const ModuleMap MODULES = {
  {"draw", Module(&draw_eval)},
  {"plot", Module(&plot_eval)},
};

ReturnCode eval(
    Context* ctx,
    const Expr* expr) {
  std::string module_name = "plot";
  ExprStorage module_args;

  auto rc = expr_walk_commands(expr, &module_args, {
    {"class", std::bind(&expr_to_string, _1, &module_name)},
    {"size", std::bind(&layer_resize_cmd, ctx, _1)},
    {"dpi", std::bind(&layer_set_dpi_cmd, ctx, _1)},
    {"font", std::bind(&layer_set_font_cmd, ctx, _1)},
    {"font-size", std::bind(&layer_set_font_size_cmd, ctx, _1)},
  });

  if (!rc) {
    return rc;
  }

  if (module_name.empty()) {
    return error(
        ERROR,
        "Please specify a module:\n"
        "\n"
        "    (class <module>)\n"
        "\n"
        "List of available modules:\n"
        "\n"
        "    - plot\n");
  }

  const Module* module;
  if (auto mod_iter = MODULES.find(module_name); mod_iter != MODULES.end()) {
    module = &mod_iter->second;
  } else {
    return {ERROR, fmt::format("Invalid module '{}'", module_name)};
  }

  if (auto rc = (*module)(ctx, &*module_args); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode eval(
    Context* ctx,
    const std::string& input) {
  ExprStorage expr;
  if (auto rc = expr_parse(input.data(), input.length(), &expr); !rc) {
    return rc;
  }

  return eval(ctx, expr.get());
}

} // namespace clip


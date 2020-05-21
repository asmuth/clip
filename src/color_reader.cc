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
#include "context.h"
#include "data.h"
#include "color_reader.h"
#include "sexpr_util.h"

using namespace std::placeholders;

namespace clip {

ReturnCode color_read(
    const Context* ctx,
    const Expr* expr,
    Color* color) {
  return expr_to_color(expr, *layer_get(ctx), color);
}

ReturnCode expr_to_color(
    const Expr* expr,
    const Layer& layer,
    Color* color) {
  if (expr_is_value(expr)) {
    const auto value = expr_get_value(expr);

    // hex codes
    if (StringUtil::beginsWith(value, "#")) {
      if (color->parse(value)) {
        return OK;
      }
    }

    // color palette index
    if (StringUtil::isDigitString(value)) {
      if (!layer.color_palette.empty()) {
        *color = layer.color_palette[
            (std::stol(value) - 1) % layer.color_palette.size()];
      }

      return OK;
    }
  }

  if (expr_is_list(expr)) {
    auto args = expr_collect(expr_get_list(expr));

    // rgb
    if (args.size() == 4 && expr_is_value(args[0], "rgb")) {
      std::array<double, 3> components;
      for (size_t i = 0; i < components.size(); ++i) {
        if (auto rc = expr_to_float64(args[i + 1], &components[i]); !rc) {
          return rc;
        }
      }

      *color = Color::fromRGB(components[0], components[1], components[2]);
      return OK;
    }

    // rgba
    if (args.size() == 5 && expr_is_value(args[0], "rgba")) {
      std::array<double, 4> components;
      for (size_t i = 0; i < components.size(); ++i) {
        if (auto rc = expr_to_float64(args[i + 1], &components[i]); !rc) {
          return rc;
        }
      }

      *color = Color::fromRGBA(
          components[0],
          components[1],
          components[2],
          components[3]);

      return OK;
    }
  }

  return errorf(
      ERROR,
      "invalid color; expected hex color code, rgb, rgba or palette index, "
      "got: '{}'",
      expr_inspect(expr));
}

ReturnCode color_read_string(
    const Context* ctx,
    const std::string& value,
    Color* color) {
  const auto layer = layer_get(ctx);

  // hex code
  if (StringUtil::beginsWith(value, "#")) {
    if (color->parse(value)) {
      return OK;
    }
  }

  // color palette index
  if (StringUtil::isDigitString(value)) {
    if (!layer->color_palette.empty()) {
      *color = layer->color_palette[std::stoul(value) % layer->color_palette.size()];
    }

    return OK;
  }

  return errorf(
      ERROR,
      "invalid color; expected hex color code or palette index, got: '{}'",
      value);
}

ReturnCode color_read_opt(
    const Context* ctx,
    const Expr* expr,
    Option<Color>* color) {
  Color c;
  if (auto rc = color_read(ctx, expr, &c); !rc) {
    return rc;
  }

  *color = c;
  return OK;
}

ReturnCode color_map_read_gradient(
    const Context* ctx,
    const Expr* expr,
    ColorMap* color_map) {
  std::vector<std::pair<double, Color>> gradient;

  for (; expr; expr = expr_next(expr)) {
    if (!expr_is_list(expr)) {
      return errorf(
          ERROR,
          "invalid argument to 'gradient'; expected a 2-tuple, but got: '{}'",
          expr_inspect(expr));
    }

    auto args = expr_collect(expr_get_list(expr));
    if (args.size() != 2) {
      return errorf(
          ERROR,
          "invalid argument to 'gradient'; expected a 2-tuple, but got: '{}'",
          expr_inspect(expr));
    }

    double step;
    if (auto rc = expr_to_ratio(args[0], &step); !rc) {
      return rc;
    }

    Color color;
    if (auto rc = color_read(ctx, args[1], &color); !rc) {
      return rc;
    }

    gradient.emplace_back(step, color);
  }

  *color_map = color_map_gradient(gradient);
  return OK;
}

ReturnCode color_map_read_steps(
    const Context* ctx,
    const Expr* expr,
    ColorMap* color_map) {
  std::vector<std::pair<double, Color>> steps;

  for (; expr; expr = expr_next(expr)) {
    if (!expr_is_list(expr)) {
      return errorf(
          ERROR,
          "invalid argument to 'steps'; expected a 2-tuple, but got: '{}'",
          expr_inspect(expr));
    }

    auto args = expr_collect(expr_get_list(expr));
    if (args.size() != 2) {
      return errorf(
          ERROR,
          "invalid argument to 'steps'; expected a 2-tuple, but got: '{}'",
          expr_inspect(expr));
    }

    double step;
    if (auto rc = expr_to_ratio(args[0], &step); !rc) {
      return rc;
    }

    Color color;
    if (auto rc = color_read(ctx, args[1], &color); !rc) {
      return rc;
    }

    steps.emplace_back(step, color);
  }

  *color_map = color_map_steps(steps);
  return OK;
}

ReturnCode color_map_read(
    const Context* ctx,
    const Expr* expr,
    ColorMap* color_map) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument to <color-map>; expected a list, but got: '{}'",
        expr_inspect(expr));
  }

  expr = expr_get_list(expr);

  if (expr_is_value(expr, "gradient")) {
    return color_map_read_gradient(ctx, expr_next(expr), color_map);
  }

  if (expr_is_value(expr, "steps")) {
    return color_map_read_steps(ctx, expr_next(expr), color_map);
  }

  return errorf(
      ERROR,
      "invalid value to <color-map>; got '{}', but expected one of: \n"
      "  - steps\n"
      "  - gradient\n",
      expr_inspect(expr));
}

ReturnCode color_palette_read(
    const Context* ctx,
    const Expr* expr,
    ColorPalette* color_palette) {
  color_palette->clear();

  if (expr_is_value(expr)) {
    return color_palette_default(expr_get_value(expr), color_palette);
  }

  if (expr_is_list(expr)) {
    Color c;
    for (auto arg = expr_get_list(expr); arg; arg = expr_next(arg)) {
      if (auto rc = color_read(ctx, arg, &c); !rc) {
        return rc;
      }

      color_palette->push_back(c);
    }

    return OK;
  }

  return errorf(
      ERROR,
      "invalid color palette; expected color palette name or color list, "
      "got: '{}'",
      expr_inspect(expr));
}

} // namespace clip


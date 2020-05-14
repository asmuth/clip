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
#include "marker.h"
#include "graphics/path.h"
#include "sexpr_conv.h"

namespace clip {

const std::unordered_map<std::string, std::string> UNICODE_MARKERS = {
  {"circle-bullet", "◉"},
  {"circle-left", "◐"},
  {"circle-right", "◑"},
  {"circle-top", "◓"},
  {"circle-bottom", "◒"},
  {"circle-bullet-alt", "⭗"},
  {"circle-hatch", "◍"},
  {"circle-dashed", "◌"},
  {"diamond", "◆"},
  {"diamond-o", "◇"},
  {"diamond-bullet", "◈"},
  {"diamond-left", "⬖"},
  {"diamond-right", "⬗"},
  {"diamond-top", "⬘"},
  {"diamond-bottom", "⬙"},
  {"square", "■"},
  {"square-o", "□"},
  {"square-left", "◧"},
  {"square-right", "◨"},
  {"square-top", "⬒"},
  {"square-bottom", "⬓"},
  {"square-diag-left", "◩"},
  {"square-diag-right", "◪"},
  {"square-dotted", "⬚"},
  {"square-crosshair", "⯐"},
  {"square-bullet", "▣"},
  {"square-hatch-horiz", "▤"},
  {"square-hatch-vert", "▥"},
  {"square-hatch-cross", "▦"},
  {"square-hatch-diag-left", "▧"},
  {"square-hatch-diag-right", "▨"},
  {"hexagon", "⬢"},
  {"hexagon-o", "⬡"},
  {"pentagon", "⬟"},
  {"pentagon-o", "⬠"},
  {"compass-left", "⮘"},
  {"compass-right", "⮚"},
  {"compass-top", "⮙"},
  {"compass-bottom", "⮛"},
  {"triangle-left", "⯇"},
  {"triangle-right", "⯈"},
  {"triangle-top", "⯅"},
  {"triangle-bottom", "⯆"},
  {"triangle-left-o", "△"},
  {"triangle-right-o", "▷"},
  {"triangle-top-o", "△"},
  {"triangle-left-half", "◭"},
  {"triangle-right-half", "◮"},
  {"triangle-bottom-o", "▽"},
  {"triangle-dot", "◬"},
};

Marker marker_create_circle(double border_width) {
  border_width = std::clamp(border_width, 0.0, 1.0);

  return [border_width] (
      auto ctx,
      const auto& pos,
      const auto& size,
      const auto& color) {
    DrawCommand shape;
    shape.stroke_style.color = color;
    shape.stroke_style.line_width = from_unit(double(size) * border_width * 0.5);
    path_add_circle(&shape.path, pos, size * 0.5);
    draw_shape(ctx, shape);
    return OK;
  };
}

Marker marker_create_disk() {
  return [] (
      auto ctx,
      const auto& pos,
      const auto& size,
      const auto& color) {
    DrawCommand shape;
    path_add_circle(&shape.path, pos, size * 0.5);
    shape.fill_style.color = color;
    draw_shape(ctx, shape);
    return OK;
  };
}

Marker marker_create_unicode(const std::string& u) {
  return [u] (
      auto ctx,
      const auto& pos,
      const auto& size,
      const auto& color) {
    TextStyle style;
    style.font = layer_get_font(ctx);
    style.color = color;
    style.font_size = from_unit(double(size) * 1.2);

    auto ax = HAlign::CENTER;
    auto ay = VAlign::CENTER;
    if (auto rc = draw_text(ctx, u, pos, ax, ay, style); rc != OK) {
      return ERROR;
    }

    return OK;
  };
}

ReturnCode marker_configure(
    const Expr* expr,
    Marker* marker) {

  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list (<marker-shape>), but got: {}",
        expr_inspect(expr));
  }

  expr = expr_get_list(expr);

  if (expr_is_value(expr, "circle")) {
    *marker = marker_create_disk();
    return OK;
  }

  if (expr_is_value(expr, "circle-o")) {
    double border_width = 0.2;
    if (expr_next(expr)) {
      if (auto rc = expr_to_float64(expr_next(expr), &border_width); !rc) {
        return rc;
      }
    }

    *marker = marker_create_circle(border_width);
    return OK;
  }

  if (expr_is_value(expr, "custom")) {
    if (!expr_next(expr) || !expr_is_value(expr_next(expr))) {
      return error(
          ERROR,
          "the 'custom' marker constructor expects exactly one argument");
    }

    *marker = marker_create_unicode(expr_get_value(expr_next(expr)));
    return OK;
  }

  if (expr_is_value(expr)) {
    auto unicode_marker = UNICODE_MARKERS.find(expr_get_value(expr));
    if (unicode_marker != UNICODE_MARKERS.end()) {
      *marker = marker_create_unicode(unicode_marker->second);
      return OK;
    }
  }

  return errorf(
      ERROR,
      "invalid marker shape '{}'; see clip.org/documentation/marker-shapes for "
      "a list of valid shapes",
      expr_inspect(expr));
}

ReturnCode marker_configure_list(
    const Expr* expr,
    std::vector<Marker>* markers) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "invalid argument; expected a list (<marker-shape>), but got: {}",
        expr_inspect(expr));
  }

  auto arg0 = expr_get_list(expr);

  if (expr_is_list(arg0)) {
    for (auto arg = arg0; arg; arg = expr_next(arg)) {
      Marker marker;
      if (auto rc = marker_configure(arg, &marker); !rc) {
        return rc;
      }

      markers->emplace_back(std::move(marker));
    }
  } else {
    Marker marker;
    if (auto rc = marker_configure(expr, &marker); !rc) {
      return rc;
    }

    markers->emplace_back(std::move(marker));
  }

  return OK;
}

} // namespace clip


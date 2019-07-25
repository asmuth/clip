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
#include "marker.h"
#include "graphics/path.h"
#include "sexpr_conv.h"

namespace fviz {

const std::unordered_map<std::string, std::string> UNICODE_MARKERS = {
  {"hexagon", "⬢"},
  {"hexagon-o", "⬡"},
};

Marker marker_create_circle(double border_width) {
  border_width = std::clamp(border_width, 0.0, 1.0);

  return [border_width] (const auto& pos, const auto& size, const auto& color, auto target) {
    StrokeStyle style;
    style.color = color;
    style.line_width = from_unit(double(size) * border_width * 0.5);

    Path path;
    path.moveTo(pos.x + size * 0.5, pos.y);
    path.arcTo(pos.x, pos.y, size * 0.5, 0, M_PI * 2);
    strokePath(target, path, style);

    return OK;
  };
}

Marker marker_create_disk() {
  return [] (const auto& pos, const auto& size, const auto& color, auto target) {
    FillStyle style;
    style.color = color;

    Path path;
    path.moveTo(pos.x + size * 0.5, pos.y);
    path.arcTo(pos.x, pos.y, size * 0.5, 0, M_PI * 2);
    fillPath(target, path, style);

    return OK;
  };
}

Marker marker_create_unicode(const std::string& u) {
  return [u] (const auto& pos, const auto& size, const auto& color, auto target) {
    TextStyle style;
    style.font = target->font;
    style.color = color;
    style.font_size = size;

    auto ax = HAlign::CENTER;
    auto ay = VAlign::CENTER;
    if (auto rc = drawTextLabel(u, pos, ax, ay, style, target); rc != OK) {
      return rc;
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

  if (expr_is_value(expr, "disk")) {
    *marker = marker_create_disk();
    return OK;
  }

  if (expr_is_value(expr, "circle")) {
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
      "invalid marker shape '{}'; see fviz.org/documentation/marker-shapes for "
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

} // namespace fviz


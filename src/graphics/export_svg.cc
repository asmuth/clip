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
#include "config.h"
#include "context.h"
#include "graphics/shape_hatch.h"
#include "graphics/draw_cmd.h"
#include "export_svg.h"

#include <sstream>
#include <iomanip>

using std::bind;
using namespace std::placeholders;

namespace clip {

struct SVGData {
  std::stringstream buffer;
  double width;
  double height;
  mat3 proj;
};

using SVGDataRef = std::shared_ptr<SVGData>;

std::string svg_number(double num) {
  double num_int;
  if (std::modf(num, &num_int) == 0) {
    return std::to_string(int(num_int));
  }

  std::stringstream buf;
  buf
    << std::fixed
    << std::setprecision(3)
    << num;

  return buf.str();
}

std::string svg_attr(const std::string& name, const std::string& val) {
  std::string buf = " ";

  buf += name;
  buf += "=\"";

  for (const auto& c : val) {
    switch (c) {
      case '\"':
        buf += "\\\"";
        break;
      default:
        buf += c;
        break;
    }
  }

  buf += "\"";
  return buf;
}

std::string svg_attr(const std::string& name, const std::string& val, double pred) {
  if (pred) {
    return svg_attr(name, val);
  } else {
    return {};
  }
}

std::string svg_attr(const std::string& name, double val) {
  return svg_attr(name, svg_number(val));
}

std::string svg_attr(const std::string& name, double val, bool pred) {
  if (pred) {
    return svg_attr(name, svg_number(val));
  } else {
    return {};
  }
}

std::string svg_body(const std::string& in) {
  std::string out;
  for (const auto& c : in) {
    switch (c) {
      case '&':
        out += "&amp;";
        break;
      case '<':
        out += "&lt;";
        break;
      case '>':
        out += "&gt;";
        break;
      default:
        out += c;
        break;
    }
  }

  return out;
}

std::string svg_path_data(const Path& path) {
  std::stringstream path_data;
  for (size_t i = 0; i < path.size(); ++i) {
    if (i > 0) {
      path_data << " ";
    }

    const auto& cmd = path[i];
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        path_data << fmt::format(
            "M{} {}",
            svg_number(cmd[0]),
            svg_number(cmd[1]));
        break;
      case PathCommand::LINE_TO:
        path_data << fmt::format(
            "L{} {}",
            svg_number(cmd[0]),
            svg_number(cmd[1]));
        break;
      case PathCommand::QUADRATIC_CURVE_TO:
        path_data << fmt::format(
            "Q{} {} {} {}",
            svg_number(cmd[0]),
            svg_number(cmd[1]),
            svg_number(cmd[2]),
            svg_number(cmd[3]));
        break;
      case PathCommand::CUBIC_CURVE_TO:
        path_data << fmt::format(
            "C{} {} {} {} {} {}",
            svg_number(cmd[0]),
            svg_number(cmd[1]),
            svg_number(cmd[2]),
            svg_number(cmd[3]),
            svg_number(cmd[4]),
            svg_number(cmd[5]));
        break;
      case PathCommand::CLOSE:
        path_data << "Z";
    }
  }

  return path_data.str();
}

std::string svg_poly_data(const Polygon2& poly) {
  if (poly.vertices.empty()) {
    return "";
  }

  std::stringstream poly_data;
  for (const auto& v : poly.vertices) {
    poly_data << svg_number(v.x);
    poly_data << " ";
    poly_data << svg_number(v.y);
  }

  return poly_data.str();
}

std::string svg_antialiasing_attrs(Option<AntialiasingMode> antialiasing_mode) {
  if (antialiasing_mode &&
      *antialiasing_mode == AntialiasingMode::DISABLE) {
    return svg_attr("shape-rendering", "crispEdges");
  } else {
    return {};
  }
}

Status svg_add_path_compound(
    const Path& path,
    const draw_style::fill_solid& fill_style,
    const draw_style::stroke_solid& stroke_style,
    Option<AntialiasingMode> antialiasing_mode,
    SVGDataRef svg) {
  svg->buffer
      << "  "
      << "<path"
      << svg_attr("d", svg_path_data(path_transform(path, svg->proj)))
      << svg_attr("fill", fill_style.color.to_hex_str(), !color_is_black(fill_style.color))
      << svg_attr("fill-opacity", fill_style.color.component(3), fill_style.color.component(3) != 1)
      << svg_attr("stroke", stroke_style.color.to_hex_str())
      << svg_attr("stroke-opacity", stroke_style.color.component(3), stroke_style.color.component(3) != 1)
      << svg_attr("stroke-width", stroke_style.width.value)
      << svg_antialiasing_attrs(antialiasing_mode)
      << "/>"
      << "\n";

  return OK;
}

Status svg_add_path_fill_solid(
    const Path& path,
    const draw_style::fill_solid& style,
    Option<AntialiasingMode> antialiasing_mode,
    SVGDataRef svg) {
  svg->buffer
      << "  "
      << "<path"
      << svg_attr("d", svg_path_data(path_transform(path, svg->proj)))
      << svg_attr("fill", style.color.to_hex_str(), !color_is_black(style.color))
      << svg_attr("fill-opacity", style.color.component(3), style.color.component(3) != 1)
      << svg_antialiasing_attrs(antialiasing_mode)
      << "/>"
      << "\n";

  return OK;
}

Status svg_add_path_fill_hatch(
    const Path& path,
    const draw_style::fill_hatch& style,
    Option<AntialiasingMode> antialiasing_mode,
    SVGDataRef svg) {
  auto hatched = shape_hatch(
      path_to_polygon_simple(path),
      style.angle_deg,
      style.offset.value,
      style.stride.value,
      style.width.value);

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("d", svg_path_data(path_transform(hatched, svg->proj)))
      << svg_attr("fill", style.color.to_hex_str(), !color_is_black(style.color))
      << svg_attr("fill-opacity", style.color.component(3), style.color.component(3) != 1)
      << svg_antialiasing_attrs(antialiasing_mode)
      << "/>"
      << "\n";

  return OK;
}

Status svg_add_path_stroke_solid(
    const Path& path,
    const draw_style::stroke_solid& style,
    Option<AntialiasingMode> antialiasing_mode,
    SVGDataRef svg) {
  svg->buffer
      << "  "
      << "<path"
      << svg_attr("d", svg_path_data(path_transform(path, svg->proj)))
      << svg_attr("fill", "none")
      << svg_attr("stroke", style.color.to_hex_str())
      << svg_attr("stroke-opacity", style.color.component(3), style.color.component(3) != 1)
      << svg_attr("stroke-width", style.width.value)
      << svg_antialiasing_attrs(antialiasing_mode)
      << "/>"
      << "\n";

  return OK;
}

Status svg_add_path_stroke_dash(
    const Path& path,
    const draw_style::stroke_dash& style,
    Option<AntialiasingMode> antialiasing_mode,
    SVGDataRef svg) {
  std::string svg_dash_pattern;
  for (const auto& v : style.pattern) {
    if (!svg_dash_pattern.empty()) {
      svg_dash_pattern += " ";
    }

    svg_dash_pattern += svg_number(v.value);
  }

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("d", svg_path_data(path_transform(path, svg->proj)))
      << svg_attr("fill", "none")
      << svg_attr("stroke", style.color.to_hex_str())
      << svg_attr("stroke-opacity", style.color.component(3), style.color.component(3) != 1)
      << svg_attr("stroke-width", style.width.value)
      << svg_attr("stroke-dasharray", svg_dash_pattern)
      << svg_attr("stroke-dashoffset", style.offset.value)
      << svg_antialiasing_attrs(antialiasing_mode)
      << "/>"
      << "\n";

  return OK;
}

Status svg_add_path(
    const Path& path,
    const draw_style::compound& style,
    Option<AntialiasingMode> aa_mode,
    SVGDataRef svg) {
  // compound fill_solid + stroke_solid
  if (style.fill_solid.size() == 1 &&
      style.fill_hatch.empty() &&
      style.stroke_solid.size() == 1 &&
      style.stroke_dash.empty()) {
    return svg_add_path_compound(
        path,
        style.fill_solid[0],
        style.stroke_solid[0],
        aa_mode,
        svg);
  }

  // fill_solid
  for (const auto& s : style.fill_solid) {
    if (auto rc = svg_add_path_fill_solid(path, s, aa_mode, svg); !rc) {
      return rc;
    }
  }

  // fill_hatch
  for (const auto& s : style.fill_hatch) {
    if (auto rc = svg_add_path_fill_hatch(path, s, aa_mode, svg); !rc) {
      return rc;
    }
  }

  // stroke_solid
  for (const auto& s : style.stroke_solid) {
    if (auto rc = svg_add_path_stroke_solid(path, s, aa_mode, svg); !rc) {
      return rc;
    }
  }

  // stroke_dash
  for (const auto& s : style.stroke_dash) {
    if (auto rc = svg_add_path_stroke_dash(path, s, aa_mode, svg); !rc) {
      return rc;
    }
  }

  return OK;
}

Status svg_add_path(
    const DrawCommand& elem,
    SVGDataRef svg) {
  return svg_add_path(
      elem.path,
      elem.style,
      elem.antialiasing_mode,
      svg);
}

Status svg_add_text_elem_native(
    const TextInfo& elem,
    SVGDataRef svg) {
  const auto& style = elem.style;
  auto origin = mul(svg->proj, vec3{elem.origin, 1});

  std::string transform;
  if (elem.transform) {
    auto transform_m = mul(
        mul(scale2({1, -1}), translate2({0, -svg->height})),
        mul(*elem.transform, svg->proj));

    transform += svg_attr(
        "transform",
        fmt::format(
            "matrix({} {} {} {} {} {})",
            transform_m.a,
            transform_m.d,
            transform_m.b,
            transform_m.e,
            transform_m.c,
            transform_m.f));
  }

  svg->buffer
    << "  "
    << "<text"
    << svg_attr("x", origin.x)
    << svg_attr("y", origin.y)
    << svg_attr("fill", style.color.to_hex_str(), !color_is_black(style.color))
    << svg_attr("fill-opacity", style.color.component(3), style.color.component(3) != 1)
    << svg_attr("font-size", style.font_size.value)
    << svg_attr("font-family", style.font.font_family_css)
    << svg_attr("font-weight", style.font.font_weight_css)
    << transform
    << ">"
    << svg_body(elem.text)
    << "</text>"
    << "\n";

  return OK;
}

Status svg_add_text_elem_embed(
    const TextInfo& elem,
    double dpi,
    SVGDataRef svg) {
  const auto& style = elem.style;

  for (const auto& gg : elem.glyphs) {
    for (const auto& g : gg.glyphs) {
      auto gt = translate2({g.x, g.y});
      if (elem.transform) {
        gt = mul(*elem.transform, gt);
      }

      gt = mul(svg->proj, gt);

      Path gp;
      auto rc = font_get_glyph_path(
          g.font,
          unit_to_pt(elem.style.font_size, dpi),
          dpi,
          g.codepoint,
          &gp);

      if (!rc) {
        return ERROR;
      }

      svg->buffer
          << "  "
          << "<path"
          << svg_attr("fill", style.color.to_hex_str(), !color_is_black(style.color))
          << svg_attr("fill-opacity", style.color.component(3), style.color.component(3) != 1)
          << svg_attr("d", svg_path_data(path_transform(gp, gt)))
          << "/>"
          << "\n";
    }
  }

  return OK;
}

struct SVGDrawOp {
  std::function<ReturnCode (SVGDataRef svg)> draw_fn;
  uint32_t draw_idx;
};

ReturnCode export_svg(
    const Layer* layer,
    std::string* buffer) {
  auto width = layer_get_width(*layer).value;
  auto height = layer_get_height(*layer).value;

  auto svg = std::make_shared<SVGData>();
  svg->width = width;
  svg->height = height;
  svg->proj = mul(translate2({0, height}), scale2({1, -1}));

  for (const auto& cmd : layer->drawlist) {
    if (auto rc = svg_add_path(cmd, svg); !rc) {
      return rc;
    }
  }

  std::stringstream svg_doc;

  svg_doc
    << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    << fmt::format("<!-- Generated by clip v{} (clip-lang.org) -->\n", CLIP_VERSION)

    << "<svg"
      << svg_attr("xmlns", "http://www.w3.org/2000/svg")
      << svg_attr("width", width)
      << svg_attr("height", height)
      << ">\n"
    << "  <rect"
      << svg_attr("width", width)
      << svg_attr("height", height)
      << svg_attr("fill", layer->background_color.to_hex_str())
      << svg_attr("fill-opacity", layer->background_color.component(3))
      << "/>\n"
    << svg->buffer.str()
    << "</svg>\n";

  *buffer = svg_doc.str();
  return OK;
}

} // namespace clip


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
#include "page_export_svg.h"

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

std::string svg_attr(const std::string& name, double val) {
  return svg_attr(name, std::to_string(val));
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
  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        path_data << fmt::format("M{} {} ", cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        path_data << fmt::format("L{} {} ", cmd[0], cmd[1]);
        break;
      case PathCommand::QUADRATIC_CURVE_TO:
        path_data << fmt::format(
            "Q{} {} {} {} ",
            cmd[0],
            cmd[1],
            cmd[2],
            cmd[3]);
        break;
      case PathCommand::CUBIC_CURVE_TO:
        path_data << fmt::format(
            "C{} {} {} {} {} {} ",
            cmd[0],
            cmd[1],
            cmd[2],
            cmd[3],
            cmd[4],
            cmd[5]);
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
    poly_data << fmt::format("{} {} ", v.x, v.y);
  }

  return poly_data.str();
}

Status svg_shape(
    const PageShapeElement& elem,
    SVGDataRef svg) {
  std::string fill_opts;
  std::string stroke_opts;

  if (elem.fill_color) {
    fill_opts = svg_attr("fill", elem.fill_color->to_hex_str(4));
  } else {
    fill_opts = svg_attr("fill", "none");
  }

  if (elem.stroke_style.line_width) {
    stroke_opts += svg_attr("stroke-width", elem.stroke_style.line_width);
    stroke_opts += svg_attr("stroke", elem.stroke_style.color.to_hex_str(4));

    switch (elem.stroke_style.dash_type) {
      case StrokeStyle::SOLID:
        break;
      case StrokeStyle::DASH: {
        std::string dash_pattern;
        for (const auto& v : elem.stroke_style.dash_pattern) {
          dash_pattern += fmt::format("{} ", v);
        }

        stroke_opts += svg_attr("stroke-dasharray", dash_pattern);
        stroke_opts += svg_attr("stroke-dashoffset", elem.stroke_style.dash_offset);
        break;
      }
    }
  }

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("d", svg_path_data(path_transform(elem.path, svg->proj)))
      << fill_opts
      << stroke_opts
      << "/>"
      << "\n";

  return OK;
}

Status svg_text_span_native(
    const PageTextElement& elem,
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
    << svg_attr("fill", style.color.to_hex_str(4))
    << svg_attr("font-size", style.font_size)
    << svg_attr("font-family", style.font.font_family_css)
    << svg_attr("font-weight", style.font.font_weight_css)
    << transform
    << ">"
    << svg_body(elem.text)
    << "</text>"
    << "\n";

  return OK;
}

Status svg_text_span_embed(
    const PageTextElement& elem,
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
          elem.style.font_size,
          dpi,
          g.codepoint,
          &gp);

      if (!rc) {
        return ERROR;
      }

      svg->buffer
          << "  "
          << "<path"
          << svg_attr("fill", style.color.to_hex_str(4))
          << svg_attr("d", svg_path_data(path_transform(gp, gt)))
          << "/>"
          << "\n";
    }
  }

  return OK;
}

Status svg_text_span(
    const PageTextElement& elem,
    double dpi,
    SVGDataRef svg) {
  if (elem.style.font.font_family_css.empty()) {
    return svg_text_span_embed(elem, dpi, svg);
  } else {
    return svg_text_span_native(elem, svg);
  }
}

struct SVGDrawOp {
  std::function<ReturnCode (SVGDataRef svg)> draw_fn;
  uint32_t draw_idx;
};

ReturnCode page_export_svg(
    const Page& page,
    std::string* buffer) {
  // create a list of drawing operations
  std::vector<SVGDrawOp> svg_ops;
  for (const auto& e : page.text_elements) {
    SVGDrawOp svg_op;
    svg_op.draw_fn = bind(&svg_text_span, e, page.dpi, _1);
    svg_op.draw_idx = e.zindex.value_or(0);
    svg_ops.push_back(svg_op);
  }

  for (const auto& e : page.shape_elements) {
    SVGDrawOp svg_op;
    svg_op.draw_fn = bind(&svg_shape, e, _1);
    svg_op.draw_idx = e.zindex.value_or(0);
    svg_ops.push_back(svg_op);
  }

  // sort and execute operations
  std::sort(
      svg_ops.begin(),
      svg_ops.end(),
      [] (const auto& a, const auto& b) {
        return a.draw_idx < b.draw_idx;
      });

  auto svg = std::make_shared<SVGData>();
  svg->width = page.width;
  svg->height = page.height;
  svg->proj = mul(translate2({0, page.height}), scale2({1, -1}));

  for (const auto& op : svg_ops) {
    if (auto rc = op.draw_fn(svg); !rc) {
      return rc;
    }
  }

  // return the svg document
  std::stringstream svg_doc;

  svg_doc
    << "<svg"
      << svg_attr("xmlns", "http://www.w3.org/2000/svg")
      << svg_attr("width", page.width)
      << svg_attr("height", page.height)
      << ">\n"
    << "  "
    << "<rect"
      << svg_attr("width", page.width)
      << svg_attr("height", page.height)
      << svg_attr("fill", page.background_color.to_hex_str(4))
      << "/>\n"
    << svg->buffer.str()
    << "</svg>";

  *buffer = svg_doc.str();
  return OK;
}

} // namespace clip


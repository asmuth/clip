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
#include "layer_svg.h"
#include "utils/fileutil.h"
#include "utils/exception.h"

namespace fviz {

struct SVGData {
  std::stringstream buffer;
  double width;
  double height;
  std::string to_svg() const;
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
        path_data << fmt::format("Q{} {} {} {} ", cmd[0], cmd[1], cmd[2], cmd[3]);
        break;
      case PathCommand::CUBIC_CURVE_TO:
        path_data << fmt::format("C{} {} {} {} {} {} ", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
        break;
      case PathCommand::ARC_TO:
        // FIXME: respect angle1/2 arguments
        path_data << fmt::format("M{} {} ", cmd[0] - cmd[2], cmd[1]);
        path_data << fmt::format("a{} {} 0 1 0 {} 0 ", cmd[2], cmd[2], cmd[2] * 2);
        path_data << fmt::format("a{} {} 0 1 0 {} 0 ", cmd[2], cmd[2], -cmd[2] * 2);
        break;
      case PathCommand::CLOSE:
        path_data << "Z";
      default:
        break; // not yet implemented
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

Status svg_stroke_path(
    const layer_ops::BrushStrokeOp& op,
    SVGDataRef svg) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;

  std::string dash_opts;
  switch (style.dash_type) {
    case StrokeStyle::SOLID:
      break;
    case StrokeStyle::DASH: {
      std::string dash_pattern;
      for (const auto& v : style.dash_pattern) {
        dash_pattern += fmt::format("{} ", v);
      }

      dash_opts += svg_attr("stroke-dasharray", dash_pattern);
      dash_opts += svg_attr("stroke-dashoffset", style.dash_offset);
      break;
    }
  }

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("stroke-width", style.line_width)
      << svg_attr("stroke", style.color.to_hex_str())
      << svg_attr("fill", "none")
      << svg_attr("d", svg_path_data(path))
      << dash_opts
      << "/>"
      << "\n";

  return OK;
}

Status svg_fill_path(
    const layer_ops::BrushFillOp& op,
    SVGDataRef svg) {
  const auto& clip = op.clip;
  const auto& color = op.color;

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("fill", color.to_hex_str())
      << svg_attr("d", svg_path_data(op.path))
      << "/>"
      << "\n";

  return OK;
}

Status svg_text_span_native(
    const layer_ops::TextSpanOp& op,
    SVGDataRef svg) {
  const auto& style = op.style;

  std::string transform;
  if (op.rotate) {
    transform = svg_attr(
        "transform",
        fmt::format("rotate({} {} {})",
        op.rotate,
        op.rotate_pivot.x,
        op.rotate_pivot.y));
  }

  svg->buffer
    << "  "
    << "<text"
    << svg_attr("x", op.origin.x)
    << svg_attr("y", op.origin.y)
    << svg_attr("fill", style.color.to_hex_str())
    << svg_attr("font-size", style.font_size)
    << svg_attr("font-family", style.font.font_family_css)
    << svg_attr("font-weight", style.font.font_weight_css)
    << transform
    << ">"
    << svg_body(op.text)
    << "</text>"
    << "\n";

  return OK;
}

Status svg_text_span_embed(
    const layer_ops::TextSpanOp& op,
    double dpi,
    SVGDataRef svg) {
  const auto& style = op.style;

  for (const auto& gg : op.glyphs) {
    for (const auto& g : gg.glyphs) {
      Path gp;

      auto rc = font_get_glyph_path(
          g.font,
          op.style.font_size,
          dpi,
          g.codepoint,
          &gp);

      if (!rc) {
        return ERROR;
      }

      auto gt = fmt::format("translate({} {})", g.x, g.y);

      svg->buffer
          << "  "
          << "<path"
          << svg_attr("fill", style.color.to_hex_str())
          << svg_attr("d", svg_path_data(gp))
          << svg_attr("transform", gt)
          << "/>"
          << "\n";
    }
  }

  return OK;
}

Status svg_text_span(
    const layer_ops::TextSpanOp& op,
    double dpi,
    SVGDataRef svg) {
  if (op.style.font.font_family_css.empty()) {
    return svg_text_span_embed(op, dpi, svg);
  } else {
    return svg_text_span_native(op, svg);
  }
}

std::string SVGData::to_svg() const {
  std::stringstream svg;
  svg
    << "<svg"
    << svg_attr("xmlns", "http://www.w3.org/2000/svg")
    << svg_attr("width", width)
    << svg_attr("height", height)
    << svg_attr("viewBox", fmt::format("0 0 {} {}", width, height))
    << ">"
    << "\n"
    << buffer.str()
    << "</svg>";

  return svg.str();
}

ReturnCode layer_bind_svg(
    double width,
    double height,
    double dpi,
    Measure font_size,
    const Color& background_color,
    std::function<Status (const std::string&)> submit,
    LayerRef* layer) {
  auto svg = std::make_shared<SVGData>();

  svg->buffer
      << "  "
      << "<rect"
      << svg_attr("width", width)
      << svg_attr("height", height)
      << svg_attr("fill", background_color.to_hex_str())
      << "/>"
      << "\n";

  LayerRef l(new Layer());
  l->width = svg->width = width,
  l->height = svg->height = height,
  l->dpi = dpi,
  l->font_size = font_size,
  l->apply = [dpi, svg, submit] (const auto& op) {
    return std::visit([dpi, svg, submit] (auto&& op) {
      using T = std::decay_t<decltype(op)>;
      if constexpr (std::is_same_v<T, layer_ops::BrushStrokeOp>)
        return svg_stroke_path(op, svg);
      if constexpr (std::is_same_v<T, layer_ops::BrushFillOp>)
        return svg_fill_path(op, svg);
      if constexpr (std::is_same_v<T, layer_ops::TextSpanOp>)
        return svg_text_span(op, dpi, svg);
      if constexpr (std::is_same_v<T, layer_ops::SubmitOp>)
        return submit(svg->to_svg());
      else
        return ERROR;
    }, op);
  };

  *layer = std::move(l);
  return OK;
}

} // namespace fviz


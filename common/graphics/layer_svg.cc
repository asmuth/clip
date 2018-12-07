/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "layer_svg.h"
#include "utils/fileutil.h"
#include "utils/exception.h"

namespace plotfx {

struct SVGData {
  std::stringstream buffer;
  double width;
  double height;
  Colour background_colour;
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

Status svg_text_span(
    const layer_ops::TextSpanOp& op,
    const MeasureTable& measures,
    SVGDataRef svg) {
  const auto& style = op.style;

  svg->buffer
    << "  "
    << "<text"
    << svg_attr("x", op.position.x)
    << svg_attr("y", op.position.y)
    << svg_attr("fill", style.colour.to_hex_str())
    << svg_attr("font-size", to_px(measures, style.font_size).value)
    << svg_attr("font-family", style.font.font_family_css)
    << ">"
    << svg_body(op.text)
    << "</text>"
    << "\n";

  return OK;
}

std::string svg_path_data(const Path& path) {
  std::stringstream path_data;
  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        path_data << StringUtil::format("M$0 $1 ", cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        path_data << StringUtil::format("L$0 $1 ", cmd[0], cmd[1]);
        break;
      case PathCommand::ARC_TO:
        // FIXME: respect angle1/2 arguments
        path_data << StringUtil::format("M$0 $1 ", cmd[0] - cmd[2], cmd[1]);
        path_data << StringUtil::format("a$0 $0 0 1 0 $1 0 ", cmd[2], cmd[2] * 2);
        path_data << StringUtil::format("a$0 $0 0 1 0 $1 0 ", cmd[2], -cmd[2] * 2);
        break;
    }
  }

  return path_data.str();
}

Status svg_stroke_path(
    const layer_ops::BrushStrokeOp& op,
    const MeasureTable& measures,
    SVGDataRef svg) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("stroke-width", to_px(measures, style.line_width).value)
      << svg_attr("stroke", style.colour.to_hex_str())
      << svg_attr("fill", "none")
      << svg_attr("d", svg_path_data(path))
      << "/>"
      << "\n";

  return OK;
}

Status svg_fill_path(
    const layer_ops::BrushFillOp& op,
    const MeasureTable& measures,
    SVGDataRef svg) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;

  svg->buffer
      << "  "
      << "<path"
      << svg_attr("fill", style.colour.to_hex_str())
      << svg_attr("d", svg_path_data(path))
      << "/>"
      << "\n";

  return OK;
}

std::string SVGData::to_svg() const {
  std::stringstream svg;
  svg
    << "<svg"
    << svg_attr("xmlns", "http://www.w3.org/2000/svg")
    << svg_attr("viewBox", StringUtil::format("0 0 $0 $1", width, height))
    << svg_attr("viewport-fill", background_colour.to_hex_str())
    << ">"
    << "\n"
    << buffer.str()
    << "</svg>";

  return svg.str();
}

ReturnCode layer_bind_svg(
    double width,
    double height,
    const MeasureTable& measures,
    const Colour& background_colour,
    std::function<Status (const std::string&)> submit,
    LayerRef* layer) {
  auto svg = std::make_shared<SVGData>();
  svg->background_colour = background_colour;

  layer->reset(new Layer{
    .width = svg->width = width,
    .height = svg->height = height,
    .measures = measures,
    .text_shaper = std::make_shared<text::TextShaper>(),
    .apply = [svg, submit, measures] (const auto& op) {
      return std::visit([svg, submit, measures] (auto&& op) {
        using T = std::decay_t<decltype(op)>;
        if constexpr (std::is_same_v<T, layer_ops::BrushStrokeOp>)
          return svg_stroke_path(op, measures, svg);
        if constexpr (std::is_same_v<T, layer_ops::BrushFillOp>)
          return svg_fill_path(op, measures, svg);
        if constexpr (std::is_same_v<T, layer_ops::TextSpanOp>)
          return svg_text_span(op, measures, svg);
        if constexpr (std::is_same_v<T, layer_ops::SubmitOp>)
          return submit(svg->to_svg());
        else
          return ERROR_NOT_IMPLEMENTED;
      }, op);
    },
  });

  return OK;
}

} // namespace plotfx


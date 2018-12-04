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
  uint32_t width;
  uint32_t height;
  std::string to_svg() const;
};

using SVGDataRef = std::shared_ptr<SVGData>;

Status svg_text_span(
    const layer_ops::TextSpanOp& op,
    const MeasureTable& measures,
    SVGDataRef svg) {
  const auto& style = op.style;


  svg->buffer
    << "  "
    << "<text "
    << "x='" << op.x << "' "
    << "y='" << op.y << "' "
    << "fill='" << style.colour.to_hex_str() << "' "
    << "font-size='" << to_px(measures, style.font_size).value << "' "
    << "font-family='" << "\"Roboto Medium\", sans-serif" << "' "
    << ">"
    << op.text // FIXME escape
    << "</text>";

  return OK;
}

Status svg_stroke_path(
    const layer_ops::BrushStrokeOp& op,
    const MeasureTable& measures,
    SVGDataRef svg) {
  const auto& clip = op.clip;
  const auto& path = op.path;
  const auto& style = op.style;

  if (path.size() < 2) {
    return ERROR_INVALID_ARGUMENT;
  }

  svg->buffer << StringUtil::format(
      "  <path stroke-width='$0' stroke='$1' fill='none' d=\"",
      to_px(measures, style.line_width).value,
      style.colour.to_hex_str());

  for (const auto& cmd : path) {
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        svg->buffer << StringUtil::format("M$0 $1 ", cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        svg->buffer << StringUtil::format("L$0 $1 ", cmd[0], cmd[1]);
        break;
      case PathCommand::ARC_TO:
        break;
    }
  }

  svg->buffer << "\" />\n";

  return OK;
}

std::string SVGData::to_svg() const {
  return StringUtil::format(
      "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox='0 0 $0 $1' viewport-fill='white'>\n$2\n</svg>",
      width,
      height,
      buffer.str());
}

ReturnCode layer_bind_svg(
    double width,
    double height,
    const MeasureTable& measures,
    std::function<Status (const std::string&)> submit,
    LayerRef* layer) {
  auto svg = std::make_shared<SVGData>();
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


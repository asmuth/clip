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
#include "layer_pixmap.h"

namespace plotfx {

ReturnCode layer_bind_png(
    double width,
    double height,
    const MeasureTable& measures,
    const Colour& background_colour,
    LayerRef* layer) {
  auto text_shaper = std::make_shared<text::TextShaper>();
  auto raster = std::make_shared<Rasterizer>(width, height, measures, text_shaper);
  raster->clear(background_colour);

  layer->reset(new Layer {
    .width = width,
    .height = height,
    .measures = measures,
    .text_shaper = text_shaper,
    .op_brush_stroke = std::bind(
        &Rasterizer::strokePath,
        raster.get(), // safe
        std::placeholders::_1),
    .op_brush_fill = std::bind(
        &Rasterizer::fillPath,
        raster.get(), // safe
        std::placeholders::_1),
    .op_text_span = std::bind(
        &Rasterizer::drawText,
        raster.get(), // safe
        std::placeholders::_1),
    .data = [raster] { return raster->to_png(); },
  });

  return OK;
}

} // namespace plotfx


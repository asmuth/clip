/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include <iostream>
#include "brush.h"
#include "layer.h"

namespace plotfx {

void fillPath(
    Layer* layer,
    const Path& path,
    const FillStyle& style) {
  fillPath(
      layer,
      Rectangle(0, 0, layer->width, layer->height),
      path.data(),
      path.size(),
      style);
}

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const FillStyle& style) {
  fillPath(layer, clip, path.data(), path.size(), style);
}

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const PathData* point_data,
    size_t point_count,
    const FillStyle& style) {
  layer->rasterizer.fillPath(clip, point_data, point_count, style);
}


void strokePath(
    Layer* layer,
    const Path& path,
    const StrokeStyle& style) {
  strokePath(
      layer,
      Rectangle(0, 0, layer->width, layer->height),
      path.data(),
      path.size(),
      style);
}

void strokePath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const StrokeStyle& style) {
  strokePath(layer, clip, path.data(), path.size(), style);
}

void strokePath(
    Layer* layer,
    const Rectangle& clip,
    const PathData* point_data,
    size_t point_count,
    const StrokeStyle& style) {
  layer->rasterizer.strokePath(clip, point_data, point_count, style);
}

void strokeLine(
    Layer* layer,
    double x1,
    double y1,
    double x2,
    double y2,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(x1, y1);
  p.lineTo(x2, y2);
  p.lineTo(x2, y2);
  p.lineTo(x2, y2);

  Rectangle clip(0, 0, layer->width, layer->height);
  strokePath(layer, clip, p.data(), p.size(), style);
}

} // namespace plotfx


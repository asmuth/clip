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
  layer_ops::BrushFillOp op;
  op.clip = clip;
  op.path = path;
  op.style = style;

  layer->apply(op);
}

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const PathData* point_data,
    size_t point_count,
    const FillStyle& style) {
  return fillPath(layer, clip, Path(point_data, point_count), style);
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
  layer_ops::BrushStrokeOp op;
  op.clip = clip;
  op.path = path;
  op.style = style;
  layer->apply(op);
}

void strokePath(
    Layer* layer,
    const Rectangle& clip,
    const PathData* point_data,
    size_t point_count,
    const StrokeStyle& style) {
  strokePath(layer, clip, Path(point_data, point_count), style);
}

void strokeLine(
    Layer* layer,
    const Point& p1,
    const Point& p2,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(p1.x, p1.y);
  p.lineTo(p2.x, p2.y);

  Rectangle clip(0, 0, layer->width, layer->height);
  strokePath(layer, clip, p, style);
}

void strokeRectangle(
    Layer* layer,
    const Point& origin,
    double width,
    double height,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(origin.x, origin.y);
  p.lineTo(origin.x + width, origin.y);
  p.lineTo(origin.x + width, origin.y + height);
  p.lineTo(origin.x, origin.y + height);
  p.lineTo(origin.x, origin.y);

  Rectangle clip(0, 0, layer->width, layer->height);
  strokePath(layer, clip, p, style);
}

void fillRectangle(
    Layer* layer,
    const Point& origin,
    double width,
    double height,
    const FillStyle& style) {
  Path p;
  p.moveTo(origin.x, origin.y);
  p.lineTo(origin.x + width, origin.y);
  p.lineTo(origin.x + width, origin.y + height);
  p.lineTo(origin.x, origin.y + height);
  p.lineTo(origin.x, origin.y);

  Rectangle clip(0, 0, layer->width, layer->height);
  fillPath(layer, clip, p, style);
}


} // namespace plotfx


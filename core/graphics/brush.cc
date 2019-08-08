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
#include <iostream>
#include "brush.h"
#include "layer.h"

namespace fviz {

void fillPath(
    Layer* layer,
    const Path& path,
    const Color& color) {
  fillPath(
      layer,
      Rectangle(0, 0, layer->width, layer->height),
      path,
      color);
}

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const Color& color) {
  layer_ops::BrushFillOp op;
  op.clip = clip;
  op.path = path;
  op.color = color;

  layer->apply(op);
}

ReturnCode fillPath(
    Layer* layer,
    const Path& path,
    const FillStyle& style) {
  return style(path, layer);
}

ReturnCode fillPath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const FillStyle& style) {
  return style(path, layer);
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
  if (style.line_width == 0) {
    return;
  }

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
  p.closePath();

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
  p.closePath();

  Rectangle clip(0, 0, layer->width, layer->height);
  fillPath(layer, clip, p, style);
}

void fillRectangle(
    Layer* layer,
    const Point& origin,
    double width,
    double height,
    const Color& color) {
  Path p;
  p.moveTo(origin.x, origin.y);
  p.lineTo(origin.x + width, origin.y);
  p.lineTo(origin.x + width, origin.y + height);
  p.lineTo(origin.x, origin.y + height);
  p.closePath();

  Rectangle clip(0, 0, layer->width, layer->height);
  fillPath(layer, clip, p, color);
}

} // namespace fviz


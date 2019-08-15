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
#include "graphics/page_description.h"

namespace fviz {

void fillPath(
    Page* page,
    const Path& path,
    const Color& color) {
  fillPath(
      page,
      Rectangle(0, 0, page->width, page->height),
      path,
      color);
}

void fillPath(
    Page* page,
    const Rectangle& clip,
    const Path& path,
    const Color& color) {
  PageShapeElement elem;
  elem.clip = clip;
  elem.path = path;
  elem.fill_color = color;

  page_add_shape(page, elem);
}

void fillPath(
    Page* page,
    const Polygon2& poly,
    const Color& color) {
  PageShapeElement elem;
  elem.path = path_from_polygon(poly);
  elem.fill_color = color;

  page_add_shape(page, elem);
}

ReturnCode fillPath(
    Page* page,
    const Path& path,
    const FillStyle& style) {
  return style(path_to_polygon_simple(path), page);
}

ReturnCode fillPath(
    Page* page,
    const Rectangle& clip,
    const Path& path,
    const FillStyle& style) {
  return style(path_to_polygon_simple(path), page);
}

void strokePath(
    Page* page,
    const Path& path,
    const StrokeStyle& style) {
  strokePath(
      page,
      Rectangle(0, 0, page->width, page->height),
      path.data(),
      path.size(),
      style);
}

void strokePath(
    Page* page,
    const Rectangle& clip,
    const Path& path,
    const StrokeStyle& style) {
  if (style.line_width == 0) {
    return;
  }

  PageShapeElement elem;
  elem.clip = clip;
  elem.path = path;
  elem.stroke_style = style;

  page_add_shape(page, elem);
}

void strokePath(
    Page* page,
    const Rectangle& clip,
    const PathData* point_data,
    size_t point_count,
    const StrokeStyle& style) {
  strokePath(page, clip, Path(point_data, point_count), style);
}

void strokeLine(
    Page* page,
    const Point& p1,
    const Point& p2,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(p1.x, p1.y);
  p.lineTo(p2.x, p2.y);

  Rectangle clip(0, 0, page->width, page->height);
  strokePath(page, clip, p, style);
}

void strokeRectangle(
    Page* page,
    const Point& origin,
    double width,
    double height,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(origin.x,         origin.y + height);
  p.lineTo(origin.x + width, origin.y + height);
  p.lineTo(origin.x + width, origin.y);
  p.lineTo(origin.x,         origin.y);
  p.closePath();

  Rectangle clip(0, 0, page->width, page->height);
  strokePath(page, clip, p, style);
}

void fillRectangle(
    Page* page,
    const Point& origin,
    double width,
    double height,
    const FillStyle& style) {
  Path p;
  p.moveTo(origin.x,         origin.y + height);
  p.lineTo(origin.x + width, origin.y + height);
  p.lineTo(origin.x + width, origin.y);
  p.lineTo(origin.x,         origin.y);
  p.closePath();

  Rectangle clip(0, 0, page->width, page->height);
  fillPath(page, clip, p, style);
}

void fillRectangle(
    Page* page,
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

  Rectangle clip(0, 0, page->width, page->height);
  fillPath(page, clip, p, color);
}

} // namespace fviz


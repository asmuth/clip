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
#include <algorithm>
#include <iostream>
#include "layout.h"

namespace fviz {

Rectangle layout_margin_box(
    const Rectangle& parent,
    double margin_top,
    double margin_right,
    double margin_bottom,
    double margin_left) {
  Rectangle box;
  box.x = parent.x + margin_left;
  box.y = parent.y + margin_top;
  box.w = std::max(parent.w - (margin_left + margin_right), 0.0);
  box.h = std::max(parent.h - (margin_top + margin_bottom), 0.0);
  return box;
}

Point layout_align(
    const Rectangle& bbox,
    const Point& target) {
  Point p;
  p.x = (target.x - (bbox.x + bbox.w / 2));
  p.y = (target.y - (bbox.y + bbox.h / 2));
  return p;
}

Point layout_align(
    const Rectangle& bbox,
    Point target,
    HAlign align_x,
    VAlign align_y) {
  switch (align_x) {
    case HAlign::LEFT:
      target.x += bbox.w / 2;
      break;
    case HAlign::RIGHT:
      target.x -= bbox.w / 2;
      break;
    case HAlign::CENTER:
      break;
  }

  switch (align_y) {
    case VAlign::TOP:
      target.y += bbox.h / 2;
      break;
    case VAlign::BOTTOM:
      target.y -= bbox.h / 2;
      break;
    case VAlign::CENTER:
      break;
  }

  return layout_align(bbox, target);
}

} // namespace fviz


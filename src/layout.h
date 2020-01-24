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
#pragma once
#include "graphics/geometry.h"
#include "graphics/draw.h"
#include "graphics/measure.h"
#include "element.h"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <optional>

namespace clip {

struct LayoutState {

  /**
   * If set to true, constrain the size of the bounding box in the (x, y)
   * dimension, i.e. disallow it from growing
   */
  std::array<bool, 2> constraint;

  /**
   * The inner ("content") bounding box
   */
  Rectangle content_box;

};

enum class Position {
  RELATIVE,
  ABSOLUTE,
  TOP,
  RIGHT,
  BOTTOM,
  LEFT
};

struct LayoutSettings {
  LayoutSettings();
  Position position;
  std::optional<Measure> width;
  std::optional<Measure> height;
};

struct LayoutInfo {

  /**
   * The elements bounding box
   */
  Rectangle content_box;

};

struct ElementPlacement {
  ElementRef element;
  LayoutInfo layout;
};

ReturnCode layout_element(
    const LayoutSettings& config,
    double bbox_w,
    double bbox_h,
    LayoutState* parent_layout,
    Rectangle* bbox);

ReturnCode layout_elements(
    const Page& layer,
    const Rectangle& parent_bbox,
    std::vector<ElementPlacement>* elements,
    Rectangle* content_box);

ReturnCode layout_add_margins(Context* ctx, const Expr* e);

} // namespace clip


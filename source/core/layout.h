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
#pragma once
#include "graphics/geometry.h"
#include "graphics/layer.h"
#include "graphics/measure.h"
#include "element.h"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <optional>

namespace plotfx {

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
   * The outer bounding box
   */
  Rectangle bounding_box;

  /**
   * The elements bounding box
   */
  Rectangle content_box;

  /**
   * The inner bounding box
   */
  Rectangle inner_box;

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
    const Layer& layer,
    const Rectangle& parent_bbox,
    std::vector<ElementPlacement>* elements,
    Rectangle* content_box);

} // namespace plotfx


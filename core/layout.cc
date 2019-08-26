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
#include "layout.h"
#include "graphics/layout.h"

namespace clip {

LayoutSettings::LayoutSettings() : position(Position::RELATIVE) {}

ReturnCode layout_element(
    const LayoutSettings& config,
    double bbox_w,
    double bbox_h,
    LayoutState* state,
    Rectangle* bbox) {
  auto width = config.width;
  auto width_min = width.value_or(from_unit(bbox_w));
  auto width_max = width.value_or(from_unit(state->content_box.w));

  auto height = config.height;
  auto height_min = height.value_or(from_unit(bbox_h));
  auto height_max = height.value_or(from_unit(state->content_box.h));

  double extent[4] = {0, 0, 0, 0};
  switch (config.position) {

    case Position::RELATIVE:
      *bbox = Rectangle(
          state->content_box.x,
          state->content_box.y,
          width_max,
          height_max);
      break;

    case Position::TOP: {
      *bbox = Rectangle(
          state->content_box.x,
          state->content_box.y,
          width_max,
          height_min);

      extent[0] = height_min;
      break;
    }

    case Position::RIGHT: {
      *bbox = Rectangle(
          state->content_box.x + state->content_box.w - width_min,
          state->content_box.y,
          width_min,
          height_max);

      extent[1] = width_min;
      break;
    }

    case Position::BOTTOM: {
      *bbox = Rectangle(
          state->content_box.x,
          state->content_box.y + state->content_box.h - height_min,
          width_max,
          height_min);

      extent[2] = height_min;
      break;
    }

    case Position::LEFT: {
      *bbox = Rectangle(
          state->content_box.x,
          state->content_box.y,
          width_min,
          height_max);

      extent[3] = width_min;
      break;
    }

  }

  state->content_box = layout_margin_box(
      state->content_box,
      extent[0],
      extent[1],
      extent[2],
      extent[3]);

  return OK;
}

//ReturnCode layout_elements(
//    const Page& layer,
//    const Rectangle& parent_bbox,
//    std::vector<ElementPlacement>* elements,
//    Rectangle* content_box) {
//  /* sort elements by z-index */
//  std::stable_sort(
//      elements->begin(),
//      elements->end(),
//      [] (const ElementPlacement& a, const ElementPlacement& b) {
//        return a.element->z_index() < b.element->z_index();
//      });
//
//  /* compute bounding boxes */
//  LayoutState layout_state;
//  layout_state.content_box = parent_bbox;
//
//  for (auto& e : *elements) {
//    double bbox_w = 0.0;
//    double bbox_h = 0.0;
//
//    if (e.element->reflow) {
//      if (auto rc =
//            e.element->reflow(
//                layer,
//                layout_state.content_box.w,
//                layout_state.content_box.h,
//                &bbox_w,
//                &bbox_h);
//            !rc) {
//        return rc;
//      }
//    }
//
//    if (auto rc =
//          layout_element(
//              e.element->layout_settings(),
//              bbox_w,
//              bbox_h,
//              &layout_state,
//              &e.layout.content_box);
//          !rc) {
//      return rc;
//    }
//  }
//
//  for (auto& e : *elements) {
//    e.layout.bounding_box = parent_bbox;
//    e.layout.inner_box = layout_state.content_box;
//  }
//
//  *content_box = layout_state.content_box;
//  return OK;
//}

} // namespace clip


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
#include "layout.h"
#include "graphics/layout.h"

namespace plotfx {

LayoutSettings::LayoutSettings() : position(Position::RELATIVE) {}

ReturnCode layout_compute(
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
} // namespace plotfx


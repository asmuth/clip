/**
 * This file is part of the "clip" project
 *   Copyright (c) 2020 Paul Asmuth
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
#include "path.h"
#include "style_util.h"

namespace clip::draw {

ReturnCode path(const path_op& op, Layer* layer) {
  DrawCommand shape;
  shape.path = op.path;
  shape.style = op.style;

  if (!style_is_visible(shape.style)) {
    shape.style = layer->draw_default_style;
  }

  layer->drawlist.emplace_back(std::move(shape));
  return OK;
}

} // namespace clip::draw


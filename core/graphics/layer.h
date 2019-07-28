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
#pragma once
#include <stdlib.h>
#include <vector>
#include <string>

#include "color.h"
#include "fviz.h"
#include "text_shaper.h"
#include "measure.h"
#include "layer_ops.h"
#include "font_lookup.h"

namespace fviz {

/**
 * The layer is the central rendering context on which 2D vector graphics
 * operations, such as rendering text and drawing polygons, take place.
 *
 * Layers are container objects and have to be bound to a specific rendering
 * backend before they can be used. Users should create layers using one of the
 * layer_bind_* methods, for example `layer_bind_png` or `layer_bind_svg`.
 *
 * Since the layer is a low-level interface, it is recommended that you use one
 * of the many drawing helper functions, such as `strokePath` or `drawText`, instead
 * of interacting directly with the Layer. Nevertheless, all supported drawing
 * operations are available through the `apply` method of the layer. Please refer
 * to `layer_op.h` for more information on the low-level operations API.
 *
 * Once you are finished with all drawing operations, it is important to call
 * `layer_submit` to make the results visible.
 */
struct Layer {
  const double width;
  const double height;
  double dpi;
  FontInfo font;
  Measure font_size;
  std::string text_default_script;
  std::string text_default_language;
  const std::function<Status (const layer_ops::Op&)> apply;
};

using LayerRef = std::unique_ptr<Layer>;

/**
 * Submit the layer to the rendering backend. Depending on the rendering backend,
 * this will return a value to the user or swap the current screen buffer
 */
ReturnCode layer_submit(Layer* layer);

} // namespace fviz


/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include <stdlib.h>
#include <vector>
#include <string>

#include "color.h"
#include "fviz.h"
#include "text_shaper.h"
#include "measure.h"
#include "layer_ops.h"

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
  Measure font_size;
  const std::shared_ptr<text::TextShaper> text_shaper;
  const std::function<Status (const layer_ops::Op&)> apply;
};

using LayerRef = std::unique_ptr<Layer>;

/**
 * Submit the layer to the rendering backend. Depending on the rendering backend,
 * this will return a value to the user or swap the current screen buffer
 */
ReturnCode layer_submit(Layer* layer);

} // namespace fviz


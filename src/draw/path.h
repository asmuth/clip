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
#pragma once
#include <stdlib.h>
#include <string>
#include <vector>

#include "layer.h"
#include "graphics/path.h"
#include "style.h"

namespace clip::draw {

struct path_op {
  Path path;
  draw_style::compound style;
  Option<AntialiasingMode> antialiasing_mode;
};

/**
 * Draw a path
 */
ReturnCode path(const path_op& op, Layer* l);

} // namespace clip::draw


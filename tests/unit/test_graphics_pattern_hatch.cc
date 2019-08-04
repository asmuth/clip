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
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/layer.h"
#include "graphics/layer_svg.h"
#include "graphics/shape_hatch.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace fviz;

int main(int argc, char** argv) {
  LayerRef layer;
  auto rc = layer_bind_svg(
      1200,
      800,
      240,
      from_unit(12),
      Color::fromRGB(1.0, 1.0, 1.0),
      [&] (auto svg) {
      FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
      return OK;
      },
      &layer);

  auto p = shape_hatch(
      Rectangle(0, 0, 1200, 800),
      vec2(600, 400),
      45,
      0,
      150,
      70);

  StrokeStyle ss;
  ss.line_width = from_unit(70);
  strokePath(layer.get(), p, ss);

  layer_submit(layer.get());
}

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
#include "graphics/text.h"
#include "environment.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace fviz;

void draw_test(
    const Environment& env,
    Layer* l,
    uint32_t x,
    uint32_t y,
    HAlign ax,
    VAlign ay) {
  TextStyle ts;
  ts.font = env.font;
  ts.font_size = from_unit(12);

  StrokeStyle ss;
  ss.line_width = from_unit(1);

  strokeLine(l, Point(x - 10, y), Point(x + 10, y), ss);
  strokeLine(l, Point(x, y - 10), Point(x, y + 10), ss);
  drawTextLabel("Ijsselmeerdijk", Point(x, y), ax, ay, ts, l);
}

int main(int argc, char** argv) {
  Environment env;
  EXPECT_OK(environment_setup_defaults(&env));

  LayerRef layer;
  auto rc = layer_bind_svg(
      800,
      500,
      96,
      from_unit(12),
      Color::fromRGB(1.0, 1.0, 1.0),
      [&] (auto svg) {
        FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
        return OK;
      },
      &layer);

  draw_test(env, layer.get(), 100,  100, HAlign::LEFT, VAlign::TOP);
  draw_test(env, layer.get(), 400,  100, HAlign::CENTER, VAlign::TOP);
  draw_test(env, layer.get(), 700,  100, HAlign::RIGHT, VAlign::TOP);

  draw_test(env, layer.get(), 100,  200, HAlign::LEFT, VAlign::CENTER);
  draw_test(env, layer.get(), 400,  200, HAlign::CENTER, VAlign::CENTER);
  draw_test(env, layer.get(), 700,  200, HAlign::RIGHT, VAlign::CENTER);

  draw_test(env, layer.get(), 100,  300, HAlign::LEFT, VAlign::BOTTOM);
  draw_test(env, layer.get(), 400,  300, HAlign::CENTER, VAlign::BOTTOM);
  draw_test(env, layer.get(), 700,  300, HAlign::RIGHT, VAlign::BOTTOM);

  layer_submit(layer.get());
}


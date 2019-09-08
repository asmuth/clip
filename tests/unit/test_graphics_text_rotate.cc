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
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/draw.h"
#include "graphics/page_export_svg.h"
#include "graphics/text.h"
#include "environment.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace clip;

void draw_test(
    const Environment& env,
    Page* l,
    uint32_t x,
    uint32_t y,
    double a,
    HAlign ax,
    VAlign ay) {
  TextStyle ts;
  ts.font = env.font;
  ts.font_size = from_unit(12);

  StrokeStyle ss;
  ss.line_width = from_unit(1);

  strokeLine(l, Point(x - 10, y), Point(x + 10, y), ss);
  strokeLine(l, Point(x, y - 10), Point(x, y + 10), ss);
  drawTextLabel("Ijsselmeerdijk", Point(x, y), ax, ay, a, ts, l);
}

int main(int argc, char** argv) {
  Environment env;
  EXPECT_OK(environment_setup_defaults(&env));

  Page page;
  page.width = 1200;
  page.height = 800;
  page.dpi = 240;
  page.font_size = from_unit(12);
  page.background_color = Color::fromRGB(1.0, 1.0, 1.0);

  draw_test(env, &page, 100,  100, 45, HAlign::LEFT, VAlign::TOP);
  draw_test(env, &page, 400,  100, 45, HAlign::CENTER, VAlign::TOP);
  draw_test(env, &page, 700,  100, 45, HAlign::RIGHT, VAlign::TOP);

  draw_test(env, &page, 100,  200, 45, HAlign::LEFT, VAlign::CENTER);
  draw_test(env, &page, 400,  200, 45, HAlign::CENTER, VAlign::CENTER);
  draw_test(env, &page, 700,  200, 45, HAlign::RIGHT, VAlign::CENTER);

  draw_test(env, &page, 100,  300, 45, HAlign::LEFT, VAlign::BOTTOM);
  draw_test(env, &page, 400,  300, 45, HAlign::CENTER, VAlign::BOTTOM);
  draw_test(env, &page, 700,  300, 45, HAlign::RIGHT, VAlign::BOTTOM);

  std::string svg;
  EXPECT_OK(page_export_svg(page, &svg));
  FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
}


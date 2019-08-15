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
#include "graphics/page_description.h"
#include "graphics/page_export_image.h"
#include "graphics/page_export_svg.h"
#include "graphics/text.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace fviz;

void draw_test(
    Page* l,
    const FontInfo& font,
    uint32_t x,
    uint32_t y,
    TextDirection dir,
    HAlign ax,
    VAlign ay) {
  TextStyle ts;
  ts.font = font;
  ts.font_size = from_unit(28);
  ts.color = Color::fromRGB(0,0,0);
  ts.direction = dir;

  auto text = "helllo תל אביב 123 ";

  StrokeStyle ss;
  ss.line_width = from_unit(1);

  strokeLine(l, Point(x - 10, y), Point(x + 10, y), ss);
  strokeLine(l, Point(x, y - 10), Point(x, y + 10), ss);
  drawTextLabel(text, Point(x, y), ax, ay, 0, ts, l);
}

void draw_test(Page* page, const FontInfo& font) {
  draw_test(page, font, 100 * 4,  100 * 4, TextDirection::LTR, HAlign::LEFT, VAlign::TOP);
  draw_test(page, font, 400 * 4,  100 * 4, TextDirection::LTR, HAlign::CENTER, VAlign::TOP);
  draw_test(page, font, 700 * 4,  100 * 4, TextDirection::LTR, HAlign::RIGHT, VAlign::TOP);

  draw_test(page, font, 100 * 4,  200 * 4, TextDirection::LTR, HAlign::LEFT, VAlign::CENTER);
  draw_test(page, font, 400 * 4,  200 * 4, TextDirection::LTR, HAlign::CENTER, VAlign::CENTER);
  draw_test(page, font, 700 * 4,  200 * 4, TextDirection::LTR, HAlign::RIGHT, VAlign::CENTER);

  draw_test(page, font, 100 * 4,  300 * 4, TextDirection::LTR, HAlign::LEFT, VAlign::BOTTOM);
  draw_test(page, font, 400 * 4,  300 * 4, TextDirection::LTR, HAlign::CENTER, VAlign::BOTTOM);
  draw_test(page, font, 700 * 4,  300 * 4, TextDirection::LTR, HAlign::RIGHT, VAlign::BOTTOM);

  draw_test(page, font, 100 * 4,  400 * 4, TextDirection::RTL, HAlign::LEFT, VAlign::TOP);
  draw_test(page, font, 400 * 4,  400 * 4, TextDirection::RTL, HAlign::CENTER, VAlign::TOP);
  draw_test(page, font, 700 * 4,  400 * 4, TextDirection::RTL, HAlign::RIGHT, VAlign::TOP);

  draw_test(page, font, 100 * 4,  500 * 4, TextDirection::RTL, HAlign::LEFT, VAlign::CENTER);
  draw_test(page, font, 400 * 4,  500 * 4, TextDirection::RTL, HAlign::CENTER, VAlign::CENTER);
  draw_test(page, font, 700 * 4,  500 * 4, TextDirection::RTL, HAlign::RIGHT, VAlign::CENTER);

  draw_test(page, font, 100 * 4,  600 * 4, TextDirection::RTL, HAlign::LEFT, VAlign::BOTTOM);
  draw_test(page, font, 400 * 4,  600 * 4, TextDirection::RTL, HAlign::CENTER, VAlign::BOTTOM);
  draw_test(page, font, 700 * 4,  600 * 4, TextDirection::RTL, HAlign::RIGHT, VAlign::BOTTOM);
}

int main(int argc, char** argv) {
  FontRef font_ref;
  EXPECT_OK(font_load(test_data_path("fonts/LiberationSans-Regular.ttf"), &font_ref));

  FontInfo font;
  font.fonts = {font_ref};

  Page page;
  page.width = 1200;
  page.height = 800;
  page.dpi = 240;
  page.font_size = from_unit(12);
  page.background_color = Color::fromRGB(1.0, 1.0, 1.0);

  draw_test(&page, font);

  {
    std::string svg;
    EXPECT_OK(page_export_svg(page, &svg));
    FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
  }

  {
    std::string svg;
    EXPECT_OK(page_export_svg(page, &svg));
    FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
  }
}


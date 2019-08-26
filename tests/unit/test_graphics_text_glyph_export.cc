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
#include "graphics/page_description.h"
#include "graphics/page_export_svg.h"
#include "graphics/text.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace clip;

int main(int argc, char** argv) {
  FontRef font_ref;
  EXPECT_OK(font_load(test_data_path("fonts/Roboto-Medium.ttf"), &font_ref));

  FontInfo font;
  font.fonts = {font_ref};

  Page page;
  page.width = 1200;
  page.height = 800;
  page.dpi = 240;
  page.font_size = from_unit(12);
  page.background_color = Color::fromRGB(1.0, 1.0, 1.0);

  TextStyle ts;
  ts.font = font;
  ts.font_size = from_unit(120);
  ts.color = Color::fromRGB(0,0,0);
  ts.direction = TextDirection::LTR;

  drawTextLabel("6", Point(100, 100), HAlign::CENTER, VAlign::CENTER, 0, ts, &page);

  std::string svg;
  EXPECT_OK(page_export_svg(page, &svg));
  FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
}

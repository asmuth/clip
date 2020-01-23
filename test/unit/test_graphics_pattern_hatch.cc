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
#include "graphics/shape_hatch.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace clip;

int main(int argc, char** argv) {
  Path clip;
  clip.moveTo(100,        100 + 600);
  clip.lineTo(100 + 1000, 100 + 600);
  clip.lineTo(100 + 1000, 100);
  clip.lineTo(100,        100);
  clip.closePath();

  draw_cmd::Shape elem;
  elem.path = shape_hatch(path_to_polygon_simple(clip), 45, 0, 40, 20);
  elem.fill_color = Color{};

  Page page;
  page.width = 1200;
  page.height = 800;
  page.dpi = 240;
  page.font_size = from_unit(12);
  page.background_color = Color::fromRGB(1.0, 1.0, 1.0);
  draw_shape(&page, elem);

  std::string svg;
  EXPECT_OK(page_export_svg(page, &svg));
  FileUtil::write(std::string(argv[0]) + ".svg", Buffer(svg.data(), svg.size()));
}

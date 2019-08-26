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
#pragma once
#include <stdlib.h>
#include <vector>
#include <string>

#include "page_elements.h"
#include "text_layout.h"

namespace clip {

/**
 * The page structure represents store a list of abstract 2D vector graphics
 * operations, such as rendering text and drawing polygons.
 *
 * Since the page is a low-level interface, it is recommended that you use one
 * of the helper functions, such as page_add_text or page_add_shape instead of
 * interacting directly with the elements lists. Nevertheless, directly
 * manipulating the element lists is safe as long as you ensure that all elements
 * list are ordered by increasing z-index and the zindex member contains the
 * maximum z index of all elements.
 */
struct Page {
  double width;
  double height;
  double dpi;
  std::vector<PageTextElement> text_elements;
  std::vector<PageShapeElement> shape_elements;
  std::optional<uint32_t> zindex;
  FontInfo font;
  Measure font_size;
  Color background_color;
  std::string text_default_script;
  std::string text_default_language;
};

void page_add_text(Page* page, PageTextElement elem);
void page_add_shape(Page* page, PageShapeElement elem);

} // namespace clip


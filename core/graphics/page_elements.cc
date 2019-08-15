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
#include "page_description.h"

namespace fviz {

void page_add_text(Page* page, PageTextElement elem) {
  if (!elem.zindex) {
    elem.zindex = page->zindex.value_or(0) + 1;
  }

  page->zindex = std::max(*elem.zindex, page->zindex.value_or(0));
  page->text_elements.emplace_back(std::move(elem));
}

void page_add_shape(Page* page, PageShapeElement elem) {
  if (!elem.zindex) {
    elem.zindex = page->zindex.value_or(0) + 1;
  }

  page->zindex = std::max(*elem.zindex, page->zindex.value_or(0));
  page->shape_elements.emplace_back(std::move(elem));
}

} // namespace fviz


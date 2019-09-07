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
 * The page structure stores a number of global properties and a list of abstract
 * 2D vector graphics operations, such as rendering text and drawing polygons.
 *
 * Since the page is a low-level interface, it is recommended that you use one
 * of the helper functions, such as page_add_text or page_add_shape instead of
 * interacting directly with the elements lists. Nevertheless, directly
 * manipulating the structure is safe.
 */
struct Page {
  double width;
  double height;
  double dpi;
  PageElementList elements;
  Color background_color;
  FontInfo font;
  Measure font_size;
  std::string text_default_script;
  std::string text_default_language;
};

void page_add_shape(PageElementList* page_elements, PageShapeElement elem);

void page_add_path(
    PageElementList* page_elements,
    const Path& path,
    StrokeStyle stroke_style,
    FillStyle fill_style);

void page_add_line(
    PageElementList* page_elements,
    vec2 from,
    vec2 to,
    StrokeStyle stroke_style);

void page_add_text(PageElementList* page_elements, PageTextElement elem);

ReturnCode page_add_text(
    const Page& page,
    PageElementList* page_elements,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    const TextStyle& text_style);

ReturnCode page_add_text(
    const Page& page,
    PageElementList* page_elements,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    const TextStyle& text_style);

} // namespace clip


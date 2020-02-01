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

#include "draw_cmd.h"
#include "text_layout.h"

namespace clip {
struct Context;

/**
 * The draw command list represents a list of abstract 2D vector graphics
 * operations, such as rendering text and drawing polygons. Note that the "list"
 * is not necessarily a flat list, but may be a tree.
 */
using DrawCommand = std::variant<draw_cmd::Text, draw_cmd::Shape, draw_cmd::Polygon>;
using DrawCommandList = std::vector<DrawCommand>;

/**
 * The page structure stores a number of global drawing properties
 */
struct Page {
  double width;
  double height;
  double dpi;
  Color background_color;
  FontInfo font;
  Measure font_size;
  std::string text_default_script;
  std::string text_default_language;
};

void draw_shape(
    Context* ctx,
    draw_cmd::Shape elem);

void draw_path(
    Context* ctx,
    const Path& path,
    StrokeStyle stroke_style,
    FillStyle fill_style);

void draw_path(
    Context* ctx,
    const Path& path,
    StrokeStyle stroke_style,
    FillStyle fill_style);

void draw_polygon(
    Context* ctx,
    const Poly2& poly,
    StrokeStyle stroke_style,
    FillStyle fill_style);

void draw_line(
    Context* ctx,
    vec2 from,
    vec2 to,
    StrokeStyle stroke_style);

void draw_text(
    Context* ctx,
    draw_cmd::Text elem);

ReturnCode draw_text(
    Context* ctx,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    double rotate,
    TextStyle text_style);

ReturnCode draw_text(
    Context* ctx,
    const std::string& text,
    const Point& position,
    HAlign align_x,
    VAlign align_y,
    TextStyle text_style);

} // namespace clip


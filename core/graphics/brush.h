/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "color.h"
#include "path.h"
#include "measure.h"
#include "layout.h"

namespace fviz {
class Layer;

enum class StrokeLineJoin { MITER, ROUND, BEVEL };
enum class StrokeLineCap { BUTT, SQUARE, ROUND};

struct StrokeStyle {
  StrokeStyle() :
    line_join(StrokeLineJoin::MITER),
    line_cap(StrokeLineCap::SQUARE),
    color(Color::fromRGB(0, 0, 0)) {}

  Measure line_width;
  StrokeLineJoin line_join;
  StrokeLineCap line_cap;
  Color color;
};

struct FillStyle {
  FillStyle() :
    color(Color::fromRGB(0, 0, 0)) {}

  Color color;
};

void fillPath(
    Layer* layer,
    const Path& path,
    const FillStyle& style);

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const FillStyle& style);

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const PathData* path_data,
    size_t path_data_count,
    const FillStyle& style);

void strokePath(
    Layer* layer,
    const Path& path,
    const StrokeStyle& style);

void strokePath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const StrokeStyle& style);

void strokePath(
    Layer* layer,
    const Rectangle& clip,
    const PathData* path_data,
    size_t path_data_count,
    const StrokeStyle& style);

void strokeLine(
    Layer* layer,
    const Point& p1,
    const Point& p2,
    const StrokeStyle& style);

void strokeRectangle(
    Layer* layer,
    const Point& origin,
    double width,
    double height,
    const StrokeStyle& style);

void fillRectangle(
    Layer* layer,
    const Point& origin,
    double width,
    double height,
    const FillStyle& style);

} // namespace fviz


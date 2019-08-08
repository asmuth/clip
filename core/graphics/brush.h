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
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "color.h"
#include "path.h"
#include "measure.h"
#include "layout.h"
#include "style.h"

namespace fviz {
class Layer;

ReturnCode fillPath(
    Layer* layer,
    const Path& path,
    const FillStyle& style);

void fillPath(
    Layer* layer,
    const Path& path,
    const Color& color);

ReturnCode fillPath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const FillStyle& style);

void fillPath(
    Layer* layer,
    const Rectangle& clip,
    const Path& path,
    const Color& color);

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
    const Color& color);

} // namespace fviz

